/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Barrel.h"

#include "SCT_MaterialManager.h"

#include "SCT_GeometryManager.h"
#include "SCT_BarrelParameters.h"
#include "SCT_GeneralParameters.h"
#include "SCT_Identifier.h"

#include "SCT_Plane.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "TrackerGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTubs.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoShapeShift.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <iostream>

SCT_Barrel::SCT_Barrel(const std::string & name,
                       TrackerDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_logVolume = preBuild();
}


void
SCT_Barrel::getParameters()
{
  const SCT_GeneralParameters* generalParameters = m_geometryManager->generalParameters();
  m_safety = generalParameters->safety();

  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  
  m_numLayers  = parameters->numLayers(); 
  m_layerPitch = parameters->layerPitch();
  m_phiStagger = parameters->phiStagger();

  // Set numerology
  m_detectorManager->numerology().setNumLayers(m_numLayers);
  for (int i = 0; i < m_numLayers; i++)
  {
    m_detectorManager->numerology().setNumEtaModulesForLayer(i, 2);
    m_detectorManager->numerology().setNumPhiModulesForLayer(i, 4);
  }
}

const GeoLogVol * 
SCT_Barrel::preBuild()
{
  m_plane = new SCT_Plane("Plane", m_detectorManager, m_geometryManager, m_materials);
  m_width = m_plane->width() + m_safety;
  m_height = m_plane->height() + (m_numLayers - 1) * std::fabs(m_phiStagger) + m_safety;
  m_thickness = m_plane->thickness() + m_layerPitch * (m_numLayers - 1) + m_safety;

  // Create the barrel volume
  // Tube envelope containing the barrel.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_height, 0.5 * m_thickness);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());
  return stationLog;
}

GeoVPhysVol * 
SCT_Barrel::build(SCT_Identifier id)
{

  GeoFullPhysVol * barrel = new GeoFullPhysVol(m_logVolume);

  double activeDepth = m_thickness - m_safety;

  double layerThickness = m_plane->thickness();
  for (int iLayer = 0; iLayer < m_numLayers; iLayer++) {
    // Create the layers
    barrel->add(new GeoNameTag("Plane#"+intToString(iLayer)));
    barrel->add(new GeoIdentifierTag(iLayer)); // Identifier layer= iLayer
    id.setLayer(iLayer); 
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D(0.0, 
                                                                                     (iLayer - (m_numLayers-1)/2.0) * m_phiStagger ,
                                                                                     (layerThickness - activeDepth)/2 + iLayer * m_layerPitch) );
    barrel->add(transform);
    GeoVPhysVol * planePV = m_plane->build(id);
    barrel->add(planePV);
    // Store alignable transform
    m_detectorManager->addAlignableTransform(2, id.getWaferId(), transform, planePV);
  }

  // Extra Material
  // TrackerDD::ExtraMaterial xMat(m_geometryManager->distortedMatManager());
  // xMat.add(barrel, "SCTBarrel");
  
  return barrel;
  
}
