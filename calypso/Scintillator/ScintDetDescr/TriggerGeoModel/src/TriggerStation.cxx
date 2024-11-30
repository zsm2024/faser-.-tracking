/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerStation.h"

#include "TriggerMaterialManager.h"

#include "TriggerGeometryManager.h"
#include "TriggerStationParameters.h"
#include "TriggerGeneralParameters.h"
#include "TriggerIdentifier.h"
#include "TriggerPlate.h"
#include "TriggerWrapping.h"

#include "ScintReadoutGeometry/TriggerDetectorManager.h"

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

TriggerStation::TriggerStation(const std::string & name,
                         TriggerPlate* plate,
                         TriggerWrapping* wrapping,
                         ScintDD::TriggerDetectorManager* detectorManager,
                         const TriggerGeometryManager* geometryManager,
                         TriggerMaterialManager* materials)
  : TriggerUniqueComponentFactory(name, detectorManager, geometryManager, materials),
  m_plate { plate }, m_wrapping{ wrapping }
{
  getParameters();
  m_logVolume = preBuild();
}


void
TriggerStation::getParameters()
{
  const TriggerStationParameters * parameters = m_geometryManager->stationParameters();
  
  m_numPlates =   parameters->numPlates();
  m_platePitchY = parameters->platePitchY();
  m_platePitchZ = parameters->platePitchZ();

  const TriggerGeneralParameters* generalParameters = m_geometryManager->generalParameters();
  m_safety = generalParameters->safety();

  m_width  = m_plate->width() + m_safety;
  // m_length = m_plate->length() + m_safety; // veto
  // pitch includes thickness of one plate
  m_length = m_platePitchY + m_plate->length() + m_safety;
  m_thickness = (m_numPlates - 1) * std::fabs(m_platePitchZ) + m_plate->thickness() + 2 * m_wrapping->thickness() + m_safety;

  // Set numerology
  m_detectorManager->numerology().setNumPlatesPerStation(m_numPlates);
}

const GeoLogVol * 
TriggerStation::preBuild()
{
  // Create the station volume
  // Box envelope containing the station.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_length, 0.5 * m_thickness);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());
  return stationLog;
}

GeoVPhysVol * 
TriggerStation::build(TriggerIdentifier id)
{

  GeoFullPhysVol * station = new GeoFullPhysVol(m_logVolume);

  double activeDepth = m_thickness - m_safety;
  double plateThickness = m_plate->thickness();
  double wrappingThickness = m_wrapping->thickness();
  for (int iPlate = 0; iPlate < m_numPlates; iPlate++)
  {
    double plateCenterY = -0.5*m_platePitchY + (iPlate%2) * m_platePitchY;
    double plateCenterZ = -((plateThickness - activeDepth)/2 + wrappingThickness + iPlate * m_platePitchZ);

    station->add(new GeoNameTag("Wrapping#"+intToString(iPlate)+"a"));
    GeoTransform* transformA = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                    plateCenterY,
                                                                    plateCenterZ - plateThickness/2 - wrappingThickness/2));
    station->add(transformA);
    station->add(m_wrapping->getVolume());

    station->add(new GeoNameTag("Plate#"+intToString(iPlate)));
    station->add(new GeoIdentifierTag(iPlate));
    id.setPlate(iPlate);
    std::cout << "zPlate = " << plateCenterZ << std::endl;
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D(0.0, 
                                                                                     plateCenterY,
                                                                                     plateCenterZ));
    station->add(transform);
    GeoVPhysVol* platePV = m_plate->build(id);
    station->add(platePV);
    m_detectorManager->addAlignableTransform(0, id.getPlateId(), transform, platePV);

    station->add(new GeoNameTag("Wrapping#"+intToString(iPlate)+"b"));
    GeoTransform* transformB = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                    plateCenterY,
                                                                    plateCenterZ + plateThickness/2 + wrappingThickness/2));
    station->add(transformB);
    station->add(m_wrapping->getVolume());
  }  
  return station;
}

