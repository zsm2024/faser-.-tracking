/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoStation.h"

#include "VetoMaterialManager.h"

#include "VetoGeometryManager.h"
#include "VetoStationParameters.h"
#include "VetoGeneralParameters.h"
#include "VetoIdentifier.h"
#include "VetoPlate.h"
#include "VetoWrapping.h"

#include "ScintReadoutGeometry/VetoDetectorManager.h"

#include "ScintGeoModelUtils/ExtraMaterial.h"

#include "GeoModelKernel/GeoBox.h"
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

VetoStation::VetoStation(const std::string & name,
                         VetoPlate* plate,
                         VetoWrapping* wrapping,
                         ScintDD::VetoDetectorManager* detectorManager,
                         const VetoGeometryManager* geometryManager,
                         VetoMaterialManager* materials)
  : VetoUniqueComponentFactory(name, detectorManager, geometryManager, materials),
  m_plate { plate }, m_wrapping { wrapping }
{
  getParameters();
  m_logVolume = preBuild();
}


void
VetoStation::getParameters()
{
  const VetoStationParameters * parameters = m_geometryManager->stationParameters();
  
  m_numPlates =   parameters->numPlates();
  m_platePitch = parameters->platePitch();

  const VetoGeneralParameters* generalParameters = m_geometryManager->generalParameters();
  m_safety = generalParameters->safety();

  m_width  = m_plate->width() + m_safety;
  m_length = m_plate->length() + m_safety;
  // pitch includes thickness of one plate
  m_thickness = (m_numPlates - 1) * m_platePitch + m_plate->thickness() + 2 * m_wrapping->thickness() + m_safety;

  // Set numerology
  m_detectorManager->numerology().setNumPlatesPerStation(m_numPlates);
}

const GeoLogVol * 
VetoStation::preBuild()
{
  // Create the station volume
  // Box envelope containing the station.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_length, 0.5 * m_thickness);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());
  return stationLog;
}

GeoVPhysVol * 
VetoStation::build(VetoIdentifier id)
{

  GeoFullPhysVol * station = new GeoFullPhysVol(m_logVolume);

  double activeDepth = m_thickness - m_safety;
  double plateThickness = m_plate->thickness();
  double wrappingThickness = m_wrapping->thickness();

  for (int iPlate = 0; iPlate < m_numPlates; iPlate++)
  {
    double plateCenterZ = (plateThickness - activeDepth)/2 + wrappingThickness + iPlate * m_platePitch;

    station->add(new GeoNameTag("Wrapping#"+intToString(iPlate)+"a"));
    GeoTransform* transformA = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                    0.0,
                                                                    plateCenterZ - plateThickness/2 - wrappingThickness/2));
    station->add(transformA);
    station->add(m_wrapping->getVolume());

    station->add(new GeoNameTag("Plate#"+intToString(iPlate)));
    station->add(new GeoIdentifierTag(iPlate));
    id.setPlate(iPlate);
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D(0.0, 
                                                                                     0.0,
                                                                                     plateCenterZ));
    station->add(transform);
    GeoVPhysVol* platePV = m_plate->build(id);
    station->add(platePV);
    m_detectorManager->addAlignableTransform(0, id.getPlateId(), transform, platePV);

    station->add(new GeoNameTag("Wrapping#"+intToString(iPlate)+"b"));
    GeoTransform* transformB = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                    0.0,
                                                                    plateCenterZ + plateThickness/2 + wrappingThickness/2));
    station->add(transformB);
    station->add(m_wrapping->getVolume());

  }  
  return station;
}

