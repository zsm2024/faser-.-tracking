/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuStation.h"

#include "VetoNuMaterialManager.h"

#include "VetoNuGeometryManager.h"
#include "VetoNuStationParameters.h"
#include "VetoNuGeneralParameters.h"
#include "VetoNuIdentifier.h"
#include "VetoNuPlate.h"

#include "ScintReadoutGeometry/VetoNuDetectorManager.h"

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

VetoNuStation::VetoNuStation(const std::string & name,
                         VetoNuPlate* plate,
                         ScintDD::VetoNuDetectorManager* detectorManager,
                         const VetoNuGeometryManager* geometryManager,
                         VetoNuMaterialManager* materials)
  : VetoNuUniqueComponentFactory(name, detectorManager, geometryManager, materials),
  m_plate { plate }
{
  getParameters();
  m_logVolume = preBuild();
}


void
VetoNuStation::getParameters()
{
  const VetoNuStationParameters * parameters = m_geometryManager->stationParameters();
  
  m_numPlates =   parameters->numPlates();
  m_platePitch = parameters->platePitch();

  const VetoNuGeneralParameters* generalParameters = m_geometryManager->generalParameters();
  m_safety = generalParameters->safety();

  m_width  = m_plate->width() + m_safety;
  m_length = m_plate->length() + m_safety;
  // pitch includes thickness of one plate
  m_thickness = (m_numPlates - 1) * m_platePitch + m_plate->thickness() + m_safety;

  // Set numerology
  m_detectorManager->numerology().setNumPlatesPerStation(m_numPlates);
}

const GeoLogVol * 
VetoNuStation::preBuild()
{
  // Create the station volume
  // Box envelope containing the station.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_length, 0.5 * m_thickness);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());
  return stationLog;
}

GeoVPhysVol * 
VetoNuStation::build(VetoNuIdentifier id)
{

  GeoFullPhysVol * station = new GeoFullPhysVol(m_logVolume);

  double activeDepth = m_thickness - m_safety;
  double plateThickness = m_plate->thickness();
  for (int iPlate = 0; iPlate < m_numPlates; iPlate++)
  {
    station->add(new GeoNameTag("Plate#"+intToString(iPlate)));
    station->add(new GeoIdentifierTag(iPlate));
    id.setPlate(iPlate);
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D(0.0, 
                                                                                     0.0,
                                                                                     (plateThickness - activeDepth)/2 + iPlate * m_platePitch));
    station->add(transform);
    GeoVPhysVol* platePV = m_plate->build(id);
    station->add(platePV);
    m_detectorManager->addAlignableTransform(0, id.getPlateId(), transform, platePV);
  }  
  return station;
}

