/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerGeometryManager.h"

#include "ScintGeoModelUtils/DistortedMaterialManager.h"
#include "ScintIdentifier/PreshowerID.h"
#include "ScintReadoutGeometry/ScintCommonItems.h"
#include "PreshowerStationParameters.h"
#include "PreshowerPlateParameters.h"
#include "PreshowerRadiatorParameters.h"
#include "PreshowerAbsorberParameters.h"
#include "PreshowerWrappingParameters.h"
#include "PreshowerDataBase.h"
#include "PreshowerGeneralParameters.h"
#include "PreshowerGeoModel/PreshowerGeoModelAthenaComps.h"

PreshowerGeometryManager::PreshowerGeometryManager(PreshowerDataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new ScintDD::ScintCommonItems(m_athenaComps->getIdHelper());

  m_stationParameters = std::make_unique<PreshowerStationParameters>(m_rdb);
  m_plateParameters = std::make_unique<PreshowerPlateParameters>(m_rdb);
  m_radiatorParameters = std::make_unique<PreshowerRadiatorParameters>(m_rdb);
  m_absorberParameters = std::make_unique<PreshowerAbsorberParameters>(m_rdb);
  m_generalParameters = std::make_unique<PreshowerGeneralParameters>(m_rdb);
  m_wrappingParameters = std::make_unique<PreshowerWrappingParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<ScintDD::DistortedMaterialManager>();
}

PreshowerGeometryManager::~PreshowerGeometryManager()
{
}

//
// Access to run time options.
//
const PreshowerOptions & 
PreshowerGeometryManager::options() const
{
  return m_options;
}

void
PreshowerGeometryManager::setOptions(const PreshowerOptions & options)
{
  m_options = options;
}

const PreshowerGeoModelAthenaComps *
PreshowerGeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// ScintCommonItems which are passed to ScintDetectorElements.
//

const ScintDD::ScintCommonItems *
PreshowerGeometryManager::commonItems() const
{
  return m_commonItems;
}

const PreshowerStationParameters * 
PreshowerGeometryManager::stationParameters() const
{    
  return m_stationParameters.get();
}

const PreshowerPlateParameters * 
PreshowerGeometryManager::plateParameters() const
{    
  return m_plateParameters.get();
}

const PreshowerRadiatorParameters * 
PreshowerGeometryManager::radiatorParameters() const
{    
  return m_radiatorParameters.get();
}
const PreshowerAbsorberParameters * 
PreshowerGeometryManager::absorberParameters() const
{    
  return m_absorberParameters.get();
}

const PreshowerGeneralParameters * 
PreshowerGeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const PreshowerWrappingParameters * 
PreshowerGeometryManager::wrappingParameters() const
{    
  return m_wrappingParameters.get();
}

const ScintDD::DistortedMaterialManager * 
PreshowerGeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

PreshowerGeometryManager&
PreshowerGeometryManager::operator=(const PreshowerGeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_stationParameters.reset(new PreshowerStationParameters(m_rdb));
    m_plateParameters.reset(new PreshowerPlateParameters(m_rdb));
    m_radiatorParameters.reset(new PreshowerRadiatorParameters(m_rdb));
    m_absorberParameters.reset(new PreshowerAbsorberParameters(m_rdb));
    m_generalParameters.reset(new PreshowerGeneralParameters(m_rdb));
    m_wrappingParameters.reset(new PreshowerWrappingParameters(m_rdb));
    m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
  }
  return *this;
}

PreshowerGeometryManager::PreshowerGeometryManager(const PreshowerGeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_stationParameters.reset(new PreshowerStationParameters(m_rdb));
  m_plateParameters.reset(new PreshowerPlateParameters(m_rdb));
  m_radiatorParameters.reset(new PreshowerRadiatorParameters(m_rdb));
  m_absorberParameters.reset(new PreshowerAbsorberParameters(m_rdb));
  m_generalParameters.reset(new PreshowerGeneralParameters(m_rdb));
  m_wrappingParameters.reset(new PreshowerWrappingParameters(m_rdb));
  m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
}
