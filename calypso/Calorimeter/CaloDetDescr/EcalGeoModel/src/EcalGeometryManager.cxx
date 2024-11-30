/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalGeometryManager.h"

#include "CaloGeoModelUtils/DistortedMaterialManager.h"
#include "FaserCaloIdentifier/EcalID.h"
#include "CaloReadoutGeometry/CaloCommonItems.h"
#include "EcalRowParameters.h"
// #include "EcalModuleParameters.h"
#include "EcalDataBase.h"
#include "EcalGeneralParameters.h"
#include "EcalGeoModel/EcalGeoModelAthenaComps.h"

EcalGeometryManager::EcalGeometryManager(EcalDataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new CaloDD::CaloCommonItems(m_athenaComps->getIdHelper());

  m_rowParameters = std::make_unique<EcalRowParameters>(m_rdb);
  // m_moduleParameters = std::make_unique<EcalModuleParameters>(m_rdb);
  m_generalParameters = std::make_unique<EcalGeneralParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<CaloDD::DistortedMaterialManager>();
}

EcalGeometryManager::~EcalGeometryManager()
{
}

//
// Access to run time options.
//
const EcalOptions & 
EcalGeometryManager::options() const
{
  return m_options;
}

void
EcalGeometryManager::setOptions(const EcalOptions & options)
{
  m_options = options;
}

const EcalGeoModelAthenaComps *
EcalGeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// CaloCommonItems which are passed to CaloDetectorElements.
//

const CaloDD::CaloCommonItems *
EcalGeometryManager::commonItems() const
{
  return m_commonItems;
}

const EcalRowParameters * 
EcalGeometryManager::rowParameters() const
{    
  return m_rowParameters.get();
}

// const EcalModuleParameters * 
// EcalGeometryManager::moduleParameters() const
// {    
//   return m_moduleParameters.get();
// }

const EcalGeneralParameters * 
EcalGeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const CaloDD::DistortedMaterialManager * 
EcalGeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

EcalGeometryManager&
EcalGeometryManager::operator=(const EcalGeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_rowParameters.reset(new EcalRowParameters(m_rdb));
    // m_moduleParameters.reset(new EcalModuleParameters(m_rdb));
    m_generalParameters.reset(new EcalGeneralParameters(m_rdb));
    m_distortedMatManager.reset(new CaloDD::DistortedMaterialManager());
  }
  return *this;
}

EcalGeometryManager::EcalGeometryManager(const EcalGeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_rowParameters.reset(new EcalRowParameters(m_rdb));
  // m_moduleParameters.reset(new EcalModuleParameters(m_rdb));
  m_generalParameters.reset(new EcalGeneralParameters(m_rdb));
  m_distortedMatManager.reset(new CaloDD::DistortedMaterialManager());
}
