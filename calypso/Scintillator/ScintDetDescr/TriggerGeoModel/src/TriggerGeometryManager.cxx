/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerGeometryManager.h"

#include "ScintGeoModelUtils/DistortedMaterialManager.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintReadoutGeometry/ScintCommonItems.h"
#include "TriggerStationParameters.h"
#include "TriggerWrappingParameters.h"
#include "TriggerPlateParameters.h"
#include "TriggerDataBase.h"
#include "TriggerGeneralParameters.h"
#include "TriggerGeoModel/TriggerGeoModelAthenaComps.h"

TriggerGeometryManager::TriggerGeometryManager(TriggerDataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new ScintDD::ScintCommonItems(m_athenaComps->getIdHelper());

  m_stationParameters = std::make_unique<TriggerStationParameters>(m_rdb);
  m_wrappingParameters = std::make_unique<TriggerWrappingParameters>(m_rdb);
  m_plateParameters = std::make_unique<TriggerPlateParameters>(m_rdb);
  m_generalParameters = std::make_unique<TriggerGeneralParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<ScintDD::DistortedMaterialManager>();
}

TriggerGeometryManager::~TriggerGeometryManager()
{
}

//
// Access to run time options.
//
const TriggerOptions & 
TriggerGeometryManager::options() const
{
  return m_options;
}

void
TriggerGeometryManager::setOptions(const TriggerOptions & options)
{
  m_options = options;
}

const TriggerGeoModelAthenaComps *
TriggerGeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// ScintCommonItems which are passed to ScintDetectorElements.
//

const ScintDD::ScintCommonItems *
TriggerGeometryManager::commonItems() const
{
  return m_commonItems;
}

const TriggerStationParameters * 
TriggerGeometryManager::stationParameters() const
{    
  return m_stationParameters.get();
}

const TriggerWrappingParameters * 
TriggerGeometryManager::wrappingParameters() const
{    
  return m_wrappingParameters.get();
}

const TriggerPlateParameters * 
TriggerGeometryManager::plateParameters() const
{    
  return m_plateParameters.get();
}

const TriggerGeneralParameters * 
TriggerGeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const ScintDD::DistortedMaterialManager * 
TriggerGeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

TriggerGeometryManager&
TriggerGeometryManager::operator=(const TriggerGeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_stationParameters.reset(new TriggerStationParameters(m_rdb));
    m_wrappingParameters.reset(new TriggerWrappingParameters(m_rdb));
    m_plateParameters.reset(new TriggerPlateParameters(m_rdb));
    m_generalParameters.reset(new TriggerGeneralParameters(m_rdb));
    m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
  }
  return *this;
}

TriggerGeometryManager::TriggerGeometryManager(const TriggerGeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_stationParameters.reset(new TriggerStationParameters(m_rdb));
  m_wrappingParameters.reset(new TriggerWrappingParameters(m_rdb));
  m_plateParameters.reset(new TriggerPlateParameters(m_rdb));
  m_generalParameters.reset(new TriggerGeneralParameters(m_rdb));
  m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
}
