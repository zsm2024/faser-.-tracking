/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuGeometryManager.h"

#include "ScintGeoModelUtils/DistortedMaterialManager.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintReadoutGeometry/ScintCommonItems.h"
#include "VetoNuStationParameters.h"
#include "VetoNuPlateParameters.h"
#include "VetoNuRadiatorParameters.h"
#include "VetoNuDataBase.h"
#include "VetoNuGeneralParameters.h"
#include "VetoNuGeoModel/VetoNuGeoModelAthenaComps.h"

VetoNuGeometryManager::VetoNuGeometryManager(VetoNuDataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new ScintDD::ScintCommonItems(m_athenaComps->getIdHelper());

  m_stationParameters = std::make_unique<VetoNuStationParameters>(m_rdb);
  m_plateParameters = std::make_unique<VetoNuPlateParameters>(m_rdb);
  m_radiatorParameters = std::make_unique<VetoNuRadiatorParameters>(m_rdb);
  m_generalParameters = std::make_unique<VetoNuGeneralParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<ScintDD::DistortedMaterialManager>();
}

VetoNuGeometryManager::~VetoNuGeometryManager()
{
}

//
// Access to run time options.
//
const VetoNuOptions & 
VetoNuGeometryManager::options() const
{
  return m_options;
}

void
VetoNuGeometryManager::setOptions(const VetoNuOptions & options)
{
  m_options = options;
}

const VetoNuGeoModelAthenaComps *
VetoNuGeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// ScintCommonItems which are passed to ScintDetectorElements.
//

const ScintDD::ScintCommonItems *
VetoNuGeometryManager::commonItems() const
{
  return m_commonItems;
}

const VetoNuStationParameters * 
VetoNuGeometryManager::stationParameters() const
{    
  return m_stationParameters.get();
}

const VetoNuPlateParameters * 
VetoNuGeometryManager::plateParameters() const
{    
  return m_plateParameters.get();
}

const VetoNuRadiatorParameters * 
VetoNuGeometryManager::radiatorParameters() const
{    
  return m_radiatorParameters.get();
}


const VetoNuGeneralParameters * 
VetoNuGeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const ScintDD::DistortedMaterialManager * 
VetoNuGeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

VetoNuGeometryManager&
VetoNuGeometryManager::operator=(const VetoNuGeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_stationParameters.reset(new VetoNuStationParameters(m_rdb));
    m_plateParameters.reset(new VetoNuPlateParameters(m_rdb));
    m_radiatorParameters.reset(new VetoNuRadiatorParameters(m_rdb));
    m_generalParameters.reset(new VetoNuGeneralParameters(m_rdb));
    m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
  }
  return *this;
}

VetoNuGeometryManager::VetoNuGeometryManager(const VetoNuGeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_stationParameters.reset(new VetoNuStationParameters(m_rdb));
  m_plateParameters.reset(new VetoNuPlateParameters(m_rdb));
  m_radiatorParameters.reset(new VetoNuRadiatorParameters(m_rdb));
  m_generalParameters.reset(new VetoNuGeneralParameters(m_rdb));
  m_distortedMatManager.reset(new ScintDD::DistortedMaterialManager());
}
