/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Station.h"

#include "SCT_MaterialManager.h"

#include "SCT_GeometryManager.h"
#include "SCT_BarrelParameters.h"
#include "SCT_GeneralParameters.h"
#include "SCT_Identifier.h"

#include "SCT_Frame.h"
#include "SCT_Wrapping.h"

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

SCT_Station::SCT_Station(const std::string & name,
                        SCT_Wrapping* wrapping,
                        TrackerDD::SCT_DetectorManager* detectorManager,
                        const SCT_GeometryManager* geometryManager,
                        SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials),
  m_wrapping { wrapping }
{
  getParameters();
  m_logVolume = preBuild();
}


void
SCT_Station::getParameters()
{
  const SCT_GeneralParameters* generalParameters = m_geometryManager->generalParameters();
  m_safety = generalParameters->safety();

  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  
  m_numLayers  = parameters->numLayers(); 
  m_layerPitch = parameters->layerPitch();

  // Set numerology
  m_detectorManager->numerology().setNumLayers(m_numLayers);
  for (int i = 0; i < m_numLayers; i++)
  {
    m_detectorManager->numerology().setNumEtaModulesForLayer(i, 2);
    m_detectorManager->numerology().setNumPhiModulesForLayer(i, 4);
  }
}

const GeoLogVol * 
SCT_Station::preBuild()
{
  // m_plane = new SCT_Plane("Plane", m_detectorManager, m_geometryManager, m_materials);
  m_centralPlane = new SCT_Frame("CentralPlane", m_detectorManager, m_geometryManager, m_materials);
  m_width = m_centralPlane->width() + m_safety;
  m_height = m_centralPlane->height() + m_safety;
  m_thickness = m_centralPlane->thickness() + m_layerPitch * (m_numLayers - 1) + 2 * m_wrapping->thickness() + m_safety;

  m_upstreamPlane = new SCT_Frame("UpstreamPlane", m_detectorManager, m_geometryManager, m_materials);
  m_downstreamPlane = new SCT_Frame("DownstreamPlane", m_detectorManager, m_geometryManager, m_materials);
  // Create the barrel volume
  // Tube envelope containing the barrel.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_height, 0.5 * m_thickness);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());
  return stationLog;
}

GeoVPhysVol * 
SCT_Station::build(SCT_Identifier id)
{

  GeoFullPhysVol * station = new GeoFullPhysVol(m_logVolume);

  double activeDepth = m_thickness - m_safety;

  double layerThickness = m_centralPlane->thickness();
  double wrappingThickness = m_wrapping->thickness();

  station->add(new GeoNameTag("WrappingA"));
  GeoTransform* wrappingTransformA = new GeoTransform(GeoTrf::Translate3D(0.0, 0.0, (wrappingThickness - activeDepth)/2));
  station->add(wrappingTransformA);
  station->add(m_wrapping->getVolume());

  for (int iLayer = 0; iLayer < m_numLayers; iLayer++) {
    // Create the layers
    station->add(new GeoNameTag("Plane#"+intToString(iLayer)));
    station->add(new GeoIdentifierTag(iLayer)); // Identifier layer= iLayer
    id.setLayer(iLayer); 
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D(0.0, 
                                                                                     0.0 ,
                                                                                     wrappingThickness + (layerThickness - activeDepth)/2 + iLayer * m_layerPitch) );
    station->add(transform);
    SCT_Frame* thePlane {nullptr};
    switch (iLayer%3)
    {
      case 0:
        thePlane = m_upstreamPlane;
        break;
      case 1:
      default:
        thePlane = m_centralPlane;
        break;
      case 2:
        thePlane = m_downstreamPlane;
        break;
    }
    GeoVPhysVol * planePV = thePlane->build(id);
    station->add(planePV);
    // Store alignable transform
    m_detectorManager->addAlignableTransform(2, id.getWaferId(), transform, planePV);
  }
  
  station->add(new GeoNameTag("WrappingB"));
  GeoTransform* wrappingTransformB = new GeoTransform(GeoTrf::Translate3D(0.0, 0.0, (activeDepth - wrappingThickness)/2));
  station->add(wrappingTransformB);
  station->add(m_wrapping->getVolume());

  // Extra Material
  // TrackerDD::ExtraMaterial xMat(m_geometryManager->distortedMatManager());
  // xMat.add(barrel, "SCTBarrel");
  
  return station;
  
}
