/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalRow.h"

#include "EcalMaterialManager.h"

#include "EcalGeometryManager.h"
#include "EcalRowParameters.h"
// #include "EcalGeneralParameters.h"
#include "EcalIdentifier.h"
#include "EcalModule.h"

#include "CaloReadoutGeometry/EcalDetectorManager.h"

#include "CaloGeoModelUtils/ExtraMaterial.h"

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

EcalRow::EcalRow(const std::string & name,
                         EcalModule* module,
                         CaloDD::EcalDetectorManager* detectorManager,
                         const EcalGeometryManager* geometryManager,
                         EcalMaterialManager* materials)
  : EcalUniqueComponentFactory(name, detectorManager, geometryManager, materials),
  m_module { module }
{
  getParameters();
  m_logVolume = preBuild();
}


void
EcalRow::getParameters()
{
  m_numModules  = m_geometryManager->rowParameters()->numModules();
  m_moduleGap   = m_geometryManager->rowParameters()->xGap();
  m_moduleAngle = m_geometryManager->rowParameters()->yawAngle();
  m_modulePitch = m_moduleGap + m_module->width();
  m_moduleStagger = std::abs(m_module->width() * tan(m_moduleAngle * M_PI / 180.0));
  m_safety = 1.0e-7;

  m_width  = m_module->width() + (m_numModules - 1) * m_modulePitch + m_safety;
  m_height = m_module->height() + m_safety;

  // Row length (beam direction) includes an additional offset due to angle
  m_length = m_module->length() + (m_numModules - 1) * m_moduleStagger + m_safety;

  // Set numerology
  m_detectorManager->numerology().setNumModulesPerRow(m_numModules);
}

const GeoLogVol * 
EcalRow::preBuild()
{
  // Create the station volume
  // Box envelope containing the row.
  const GeoBox* stationEnvelopeShape = new GeoBox(0.5 * m_width, 0.5 * m_height, 0.5 * m_length);
  GeoLogVol* stationLog = new GeoLogVol(getName(), stationEnvelopeShape, m_materials->gasMaterial());
  return stationLog;
}

GeoVPhysVol * 
EcalRow::build(EcalIdentifier id)
{

  GeoFullPhysVol * station = new GeoFullPhysVol(m_logVolume);

  double activeWidth = m_width - m_safety;
  double moduleWidth = m_module->width();
  double activeLength = m_length - m_safety;
  double moduleLength = m_module->length();
  int iStaggerSign = ( m_moduleAngle >= 0 ? 1 : -1);
  for (int iModule = 0; iModule < m_numModules; iModule++)
  {
    station->add(new GeoNameTag("Module"));
    station->add(new GeoIdentifierTag(iModule));
    id.setModule(iModule);
    GeoAlignableTransform* transform = new GeoAlignableTransform(GeoTrf::Translate3D((moduleWidth - activeWidth)/2 + iModule * m_modulePitch, 
                                                                                     0.0,
                                                                                     iStaggerSign * ((moduleLength - activeLength)/2 + iModule * m_moduleStagger)));
    station->add(transform);
    GeoVPhysVol* modulePV = m_module->build(id);
    station->add(modulePV);
    m_detectorManager->addAlignableTransform(0, id.getModuleId(), transform, modulePV);
  }  
  
  return station;
}

