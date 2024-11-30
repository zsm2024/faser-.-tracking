/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoGeometryManager.h"

#include "ScintGeoModelUtils/DistortedMaterialManager.h"
#include "ScintIdentifier/VetoID.h"
#include "ScintReadoutGeometry/ScintCommonItems.h"
#include "VetoStationParameters.h"
#include "VetoPlateParameters.h"
#include "VetoRadiatorParameters.h"
#include "VetoWrappingParameters.h"
#include "VetoDataBase.h"
#include "VetoGeneralParameters.h"
#include "VetoGeoModel/VetoGeoModelAthenaComps.h"

VetoGeometryManager::VetoGeometryManager(VetoDataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new ScintDD::ScintCommonItems(m_athenaComps->getIdHelper());

  m_stationParameters = std::make_unique<VetoStationParameters>(m_rdb);
  m_plateParameters = std::make_unique<VetoPlateParameters>(m_rdb);
  m_radiatorParameters = std::make_unique<VetoRadiatorParameters>(m_rdb);
  m_wrappingParameters = std::make_unique<VetoWrappingParameters>(m_rdb);
  m_generalParameters = std::make_unique<VetoGeneralParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<ScintDD::DistortedMaterialManager>();
}

VetoGeometryManager::~VetoGeometryManager()
{
}

//
// Access to run time options.
//
const VetoOptions & 
VetoGeometryManager::options() const
{
  return m_options;
}

void
VetoGeometryManager::setOptions(const VetoOptions & options)
{
  m_options = options;
}

const VetoGeoModelAthenaComps *
VetoGeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// ScintCommonItems which are passed to ScintDetectorElements.
//

const ScintDD::ScintCommonItems *
VetoGeometryManager::commonItems() const
{
  return m_commonItems;
}

const VetoStationParameters * 
VetoGeometryManager::stationParameters() const
{    
  return m_stationParameters.get();
}

const VetoPlateParameters * 
VetoGeometryManager::plateParameters() const
{    
  return m_plateParameters.get();
}

const VetoRadiatorParameters * 
VetoGeometryManager::radiatorParameters() const
{    
  return m_radiatorParameters.get();
}

const VetoWrappingParameters * 
VetoGeometryManager::wrappingParameters() const
{    
  return m_wrappingParameters.get();
}


const VetoGeneralParameters * 
VetoGeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const ScintDD::DistortedMaterialManager * 
VetoGeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

VetoGeometryManager&
VetoGeometryManager::operator=(const VetoGeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_stationParameters.reset(new VetoStationParameters(m_rdb));
    m_plateParameters.reset(new VetoPlateParameters(m_rdb));
    m_radiatorParameters.reset(new VetoRadiatorParameters(m_rdb));
    m_wrappingParameters.reset(new VetoWrappingParameters(m_rdb));
    m_generalParameters.reset(new VetoGeneralParameters(m_rdb));
    m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
  }
  return *this;
}

VetoGeometryManager::VetoGeometryManager(const VetoGeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_stationParameters.reset(new VetoStationParameters(m_rdb));
  m_plateParameters.reset(new VetoPlateParameters(m_rdb));
  m_radiatorParameters.reset(new VetoRadiatorParameters(m_rdb));
  m_wrappingParameters.reset(new VetoWrappingParameters(m_rdb));
  m_generalParameters.reset(new VetoGeneralParameters(m_rdb));
  m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
}
