/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_HalfPlane.h"
#include "SCT_Identifier.h"
#include "SCT_GeometryManager.h"
#include "SCT_MaterialManager.h"
#include "SCT_BarrelParameters.h"
#include "SCT_GeneralParameters.h"
#include "SCT_Module.h"

#include "TrackerGeoModelUtils/ExtraMaterial.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

inline double sqr(double x) {return x * x;}

SCT_HalfPlane::SCT_HalfPlane(const std::string & name,
                             TrackerDD::SCT_DetectorManager* detectorManager,
                             const SCT_GeometryManager* geometryManager,
                             SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_HalfPlane::~SCT_HalfPlane()
{
  if (m_module != nullptr) delete m_module;
}

void
SCT_HalfPlane::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  const SCT_GeneralParameters * generalParameters = m_geometryManager->generalParameters();
    
  m_safety         = generalParameters->safety();
  m_etaHalfPitch   = parameters->etaHalfPitch();
  m_phiHalfPitch   = parameters->phiHalfPitch();
  m_depthHalfPitch = parameters->depthHalfPitch();
}

const GeoLogVol * 
SCT_HalfPlane::preBuild()
{
  m_module = new SCT_Module("Module", m_detectorManager, m_geometryManager, m_materials);
  //
  // Active Layer Envelope extent
  // Envelope for the active part (ie containing all the skis for the layer)
  //
  double activeWidth, activeThickness, activeLength;

  // Returns the outer dimensions of the active envelope
  activeEnvelopeExtent(activeThickness, activeWidth, activeLength);

  // Add some safety
  activeWidth  += m_safety;
  activeThickness += m_safety;
  activeLength += m_safety;

  //
  // Overall Layer Envelope extent
  //
  m_width = activeWidth;
  m_thickness = activeThickness;
  m_length = activeLength;
  //
  // Make envelope for layer
  //
  const GeoBox* halfPlaneEnvelopeBox = new GeoBox(0.5 * m_thickness, 0.5 * m_width, 0.5 * m_length);
  GeoLogVol* logVolume = new GeoLogVol(getName(), halfPlaneEnvelopeBox, m_materials->gasMaterial());

  // Check for overlap.

  return logVolume;
}

GeoVPhysVol * 
SCT_HalfPlane::build(SCT_Identifier id, int etaSign)
{
  // We make this a fullPhysVol
  GeoFullPhysVol * halfPlane = new GeoFullPhysVol(m_logVolume);

  for (int module = 0; module < 4; module++)
  {
    int iz = 1 - 2 * (module % 2);
    int iy = -3 + 2 * module;
    int ix = iz;
    int iEta = iz * etaSign;
    halfPlane->add(new GeoNameTag("Module#" + intToString(module)));
    halfPlane->add(new GeoIdentifierTag(module));
    id.setPhiModule(module);
    id.setEtaModule(iEta);
    GeoTrf::Transform3D position(GeoTrf::Translation3D(ix * m_depthHalfPitch, iy * m_phiHalfPitch, iz * m_etaHalfPitch));
    GeoAlignableTransform* transform = nullptr;
    if ( module%2 == 0 )
    {
      transform = new GeoAlignableTransform(position * GeoTrf::RotateX3D(180*Gaudi::Units::deg));
    }
    else
    {
      transform = new GeoAlignableTransform(position);
    }
    halfPlane->add(transform);
    GeoVPhysVol* modulePV = m_module->build(id);
    halfPlane->add(modulePV);
    m_detectorManager->addAlignableTransform(1, id.getWaferId(), transform, modulePV);
  }
  return halfPlane;
}

void
SCT_HalfPlane::activeEnvelopeExtent(double & dx, double & dy, double & dz)
{
  double layerThickness = 2 * m_depthHalfPitch + m_module->thickness() + m_safety;  // this is the x-coordinate (depth) of the modules
  const GeoTrf::Vector3D* refPt = m_module->env1RefPointVector();
  double moduleHalfWidth = std::fabs(-refPt->y()-m_module->env1Width()/2);
  double layerWidth = 2 * moduleHalfWidth + 6 * m_phiHalfPitch + m_safety;          // this is the y-coordinate (phi) of the modules
  double moduleHalfLength = std::fabs(-refPt->z()-m_module->env1Length()/2);
  double layerLength = 2 * moduleHalfLength + 2 * m_etaHalfPitch + m_safety;        // this is the z-coordinate (eta) of the modules

  dx = layerThickness;
  dy = layerWidth;
  dz = layerLength;
}
