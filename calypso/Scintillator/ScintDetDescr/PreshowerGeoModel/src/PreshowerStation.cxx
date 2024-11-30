/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerStation.h"

#include "PreshowerMaterialManager.h"

#include "PreshowerGeometryManager.h"
#include "PreshowerStationParameters.h"
#include "PreshowerGeneralParameters.h"
#include "PreshowerRadiatorParameters.h"
#include "PreshowerAbsorberParameters.h"
#include "PreshowerIdentifier.h"
#include "PreshowerPlate.h"
#include "PreshowerWrapping.h"

#include "ScintReadoutGeometry/PreshowerDetectorManager.h"

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

PreshowerStation::PreshowerStation(const std::string & name,
                         PreshowerPlate* plate,
                         PreshowerWrapping* wrapping,
                         ScintDD::PreshowerDetectorManager* detectorManager,
                         const PreshowerGeometryManager* geometryManager,
                         PreshowerMaterialManager* materials)
  : PreshowerUniqueComponentFactory(name, detectorManager, geometryManager, materials),
  m_plate { plate }, m_wrapping { wrapping }
{
  getParameters();
  m_logVolume = preBuild();
}


void
PreshowerStation::getParameters()
{
  const PreshowerStationParameters * parameters = m_geometryManager->stationParameters();
  
  m_numPlates =  parameters->numPlates();
  m_platePitch = parameters->platePitch();

  const PreshowerGeneralParameters* generalParameters = m_geometryManager->generalParameters();
  m_safety = generalParameters->safety();

  // Set numerology
  m_detectorManager->numerology().setNumPlatesPerStation(m_numPlates);

  // Passive radiator information
  const PreshowerRadiatorParameters* radiatorParameters = m_geometryManager->radiatorParameters();
  m_radiatorWidth  = radiatorParameters->radiatorWidth();
  m_radiatorLength = radiatorParameters->radiatorLength();
  m_radiatorThickness = radiatorParameters->radiatorThickness();
  m_radiatorMaterial = m_materials->getMaterial(radiatorParameters->radiatorMaterial());

  // Passive absorber information
  const PreshowerAbsorberParameters* absorberParameters = m_geometryManager->absorberParameters();
  m_absorberWidth  = absorberParameters->absorberWidth();
  m_absorberLength = absorberParameters->absorberLength();
  m_absorberThickness = absorberParameters->absorberThickness();
  m_absorberMaterial = m_materials->getMaterial(absorberParameters->absorberMaterial());

  // pitch includes thickness of one plate
  m_width  = std::max(std::max(m_plate->width(), m_radiatorWidth), m_absorberWidth) + m_safety;
  m_length = std::max(std::max(m_plate->length(), m_radiatorLength), m_absorberLength) + m_safety;

  // Compute air-gaps between slabs; for now assumed equal based on scintillator pitch
  m_airGap = (m_platePitch - m_plate->thickness() - m_radiatorThickness - m_absorberThickness - 2 * m_wrapping->thickness())/3;
  if (m_airGap < 0)
  {
    m_detectorManager->msg(MSG::FATAL) << "Invalid passive material geometry for preshower; air gap is negative." << endmsg;
  }

  m_thickness = (m_numPlates - 1) * m_platePitch + m_plate->thickness() + 3 * m_airGap + 2 * m_absorberThickness + m_radiatorThickness + 2 * m_wrapping->thickness() + m_safety;

}

const GeoLogVol * 
PreshowerStation::preBuild()
{
  // Create the station volume
  // Box envelope containing the station.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_length, 0.5 * m_thickness);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());

  const GeoBox* radiatorShape = new GeoBox(0.5 * m_radiatorWidth, 0.5 * m_radiatorLength, 0.5 * m_radiatorThickness);
  m_radiatorLog = new GeoLogVol("PreshowerRadiator", radiatorShape, m_radiatorMaterial);

  const GeoBox* absorberShape = new GeoBox(0.5 * m_absorberWidth, 0.5 * m_absorberLength, 0.5 * m_absorberThickness);
  m_absorberLog = new GeoLogVol("PreshowerAbsorber", absorberShape, m_absorberMaterial);

  return stationLog;
}

GeoVPhysVol * 
PreshowerStation::build(PreshowerIdentifier id)
{

  GeoFullPhysVol * station = new GeoFullPhysVol(m_logVolume);

  // double activeDepth = m_thickness - m_safety;
  double plateThickness = m_plate->thickness();
  double wrappingThickness = m_wrapping->thickness();
  for (int iPlate = 0; iPlate < m_numPlates; iPlate++)
  {
    station->add(new GeoNameTag("Absorber#"+intToString(iPlate)));
    double zA = (-m_numPlates/2.0 + iPlate) * m_platePitch;
    GeoTransform* absorberTransform = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                           0.0,
                                                                           zA));
    station->add(absorberTransform);
    GeoVPhysVol* absorberPV = new GeoPhysVol(m_absorberLog);
    station->add(absorberPV);

    station->add(new GeoNameTag("Radiator#"+intToString(iPlate)));
    double zR = zA + m_airGap + (m_absorberThickness + m_radiatorThickness)/2;
    GeoTransform* radiatorTransform = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                           0.0,
                                                                           zR));
    station->add(radiatorTransform);
    GeoVPhysVol* radiatorPV = new GeoPhysVol(m_radiatorLog);
    station->add(radiatorPV);

    station->add(new GeoNameTag("Wrapping#"+intToString(iPlate)+"a"));
    double zP = zR + m_airGap + wrappingThickness + (plateThickness + m_radiatorThickness)/2;
    double zW = zP - (plateThickness + wrappingThickness)/2;
    GeoTransform* wrappingTransformA = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                            0.0,
                                                                            zW));
    station->add(wrappingTransformA);
    station->add(m_wrapping->getVolume());

    station->add(new GeoNameTag("Plate#"+intToString(iPlate)));
    station->add(new GeoIdentifierTag(iPlate));
    id.setPlate(iPlate);
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D(0.0, 
                                                                                     0.0,
                                                                                     zP));
    station->add(transform);
    GeoVPhysVol* platePV = m_plate->build(id);
    station->add(platePV);
    m_detectorManager->addAlignableTransform(0, id.getPlateId(), transform, platePV);

    station->add(new GeoNameTag("Wrapping#"+intToString(iPlate)+"b"));
    zW = zP + (plateThickness + wrappingThickness)/2;
    GeoTransform* wrappingTransformB = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                            0.0,
                                                                            zW));
    station->add(wrappingTransformB);
    station->add(m_wrapping->getVolume());

  }

  // Now add the final downstream absorber after the last plate
  station->add(new GeoNameTag("Absorber#"+intToString(m_numPlates)));
  double zA = m_numPlates/2.0 * m_platePitch;
  GeoTransform* absorberTransform = new GeoTransform(GeoTrf::Translate3D(0.0,
                                                                         0.0,
                                                                         zA));
  station->add(absorberTransform);
  GeoVPhysVol* absorberPV = new GeoPhysVol(m_absorberLog);
  station->add(absorberPV);

  return station;
}

