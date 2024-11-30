/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Frame.h"
#include "SCT_Module.h"

#include "SCT_MaterialManager.h"

#include "SCT_GeometryManager.h"
#include "SCT_GeneralParameters.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GeoModelKernel/GeoShapeUnion.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "GaudiKernel/SystemOfUnits.h"

SCT_Frame::SCT_Frame(const std::string & name,
                     TrackerDD::SCT_DetectorManager* detectorManager,
                     const SCT_GeometryManager* geometryManager,
                     SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_logVolume = preBuild();
}

void
SCT_Frame::getParameters() 
{
  const SCT_GeneralParameters * generalParameters = m_geometryManager->generalParameters();
  const SCT_BarrelParameters* barrelParameters = m_geometryManager->barrelParameters();

  m_material  = m_materials->getMaterial(barrelParameters->frameMaterial());
  m_air       = m_materials->gasMaterial();
  m_safety    = generalParameters->safety();
  m_thickness = barrelParameters->frameThickness();
  m_width     = barrelParameters->frameWidth();
  m_height    = barrelParameters->frameHeight();
  m_etaHalfPitch   = barrelParameters->etaHalfPitch();
  m_phiHalfPitch   = barrelParameters->phiHalfPitch();
  m_depthHalfPitch = barrelParameters->depthHalfPitch();
  m_sideHalfPitch  = barrelParameters->sideHalfPitch();

  m_stagger = 0.0;
  if (getName().find("Central") != std::string::npos)
  {
      m_stagger = barrelParameters->phiStagger();
  }
  else if (getName().find("Downstream") != std::string::npos)
  {
      m_stagger = -barrelParameters->phiStagger();
  }

  m_booleans  = barrelParameters->frameBooleanVolumes();
}

const GeoLogVol*
SCT_Frame::preBuild() 
{
  m_module = new SCT_Module("Module", m_detectorManager, m_geometryManager, m_materials);

  // Make frame envelope. Just a simple box.
  // std::cout << "SCT_Frame: starting frame dimensions: " << m_width << " , " << m_height << " , " << m_thickness << std::endl;
  const GeoBox * simpleFrameShape = new GeoBox(0.5*m_width,
                                               0.5*m_height,
                                               0.5*m_thickness);

  // std::cout << "SCT_Frame: subtracting " << m_booleans.size() << " volumes." << std::endl;
  const GeoShape* frame = simpleFrameShape;
  for (SCT_BarrelParameters::FrameBoolean v : m_booleans)
  {
    // std::cout << "SCT_Frame: subtracting volume of dimensions " << v.dx << " , " << v.dy << " , " << v.dz << std::endl;
    // std::cout << "SCT_Frame: subtracting at position " << v.x << " , " << v.y << " , " << v.z << std::endl;
    const GeoBox* subtractVolume = new GeoBox(0.5*v.dx,
                                              0.5*v.dy,
                                              0.5*v.dz);
    double stagger = (v.staggered ? m_stagger : 0.0);
    frame = &(frame->subtract(*subtractVolume << GeoTrf::Translate3D(v.x, v.y + stagger, v.z)));
  }
  m_frame = new GeoLogVol(getName(), frame, m_material);

  return new GeoLogVol("Frame", simpleFrameShape, m_air);
}

GeoVPhysVol * 
SCT_Frame::build(SCT_Identifier id)
{

  GeoFullPhysVol* plane = new GeoFullPhysVol(m_logVolume);

  GeoVPhysVol* framePV = new GeoPhysVol(m_frame);
  plane->add(framePV);
  
  for (int side = 0; side < 2; side++ )
  {
    int izSide = -1 + 2 * side;
    int etaSign = -izSide;
    GeoTransform* sideTransform = new GeoTransform(GeoTrf::Translate3D(0.0, 0.0, izSide * m_sideHalfPitch ) * 
                                               GeoTrf::RotateY3D(-izSide * 90 * Gaudi::Units::deg));
    for (int module = 0; module < 4; module++)
    {
      int iz = 1 - 2 * (module % 2);
      int iy = -3 + 2 * module;
      int ix = iz;
      int iEta = iz * etaSign;
      plane->add(new GeoNameTag("Module#" + intToString(module)));
      plane->add(new GeoIdentifierTag(module + 4 * side));
      id.setPhiModule(module);
      id.setEtaModule(iEta);
      GeoTrf::Transform3D position(GeoTrf::Translation3D(-ix * m_depthHalfPitch, iy * m_phiHalfPitch + m_stagger, iz * m_etaHalfPitch));
      GeoAlignableTransform* transform = nullptr;
      if ( module%2 == 0 )
      {
        transform = new GeoAlignableTransform(position * GeoTrf::RotateX3D(180*Gaudi::Units::deg));
      }
      else
      {
        transform = new GeoAlignableTransform(position);
      }
      plane->add(sideTransform);
      plane->add(transform);
      GeoVPhysVol* modulePV = m_module->build(id);
      plane->add(modulePV);
      m_detectorManager->addAlignableTransform(1, id.getWaferId(), transform, modulePV);
    }
  }
  return plane;

}
