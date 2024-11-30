/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionGeometryManager.h"

#include "NeutrinoGeoModelUtils/DistortedMaterialManager.h"
#include "NeutrinoIdentifier/EmulsionID.h"
#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h"
#include "EmulsionFilmParameters.h"
#include "EmulsionPlatesParameters.h"
#include "EmulsionDataBase.h"
#include "EmulsionGeneralParameters.h"
#include "EmulsionSupportParameters.h"
#include "EmulsionGeoModel/EmulsionGeoModelAthenaComps.h"

EmulsionGeometryManager::EmulsionGeometryManager(EmulsionDataBase* rdb)
  : m_athenaComps{rdb->athenaComps()},
    m_rdb{rdb}
{
  // This class uses reference counting. Should not be delete'd in destructor.
  m_commonItems = new NeutrinoDD::NeutrinoCommonItems(m_athenaComps->getIdHelper());

  m_filmParameters = std::make_unique<EmulsionFilmParameters>(m_rdb);
  m_platesParameters = std::make_unique<EmulsionPlatesParameters>(m_rdb);
  m_generalParameters = std::make_unique<EmulsionGeneralParameters>(m_rdb);
  m_supportParameters = std::make_unique<EmulsionSupportParameters>(m_rdb);
  m_distortedMatManager = std::make_unique<NeutrinoDD::DistortedMaterialManager>();
}

EmulsionGeometryManager::~EmulsionGeometryManager()
{
}

//
// Access to run time options.
//
const EmulsionOptions & 
EmulsionGeometryManager::options() const
{
  return m_options;
}

void
EmulsionGeometryManager::setOptions(const EmulsionOptions & options)
{
  m_options = options;
}

const EmulsionGeoModelAthenaComps *
EmulsionGeometryManager::athenaComps() const 
{
  return m_athenaComps;
}
  
//
// NeutrinoCommonItems which are passed to NeutrinoDetectorElements.
//

const NeutrinoDD::NeutrinoCommonItems *
EmulsionGeometryManager::commonItems() const
{
  return m_commonItems;
}

const EmulsionFilmParameters * 
EmulsionGeometryManager::filmParameters() const
{    
  return m_filmParameters.get();
}

const EmulsionPlatesParameters * 
EmulsionGeometryManager::platesParameters() const
{    
  return m_platesParameters.get();
}

const EmulsionGeneralParameters * 
EmulsionGeometryManager::generalParameters() const
{    
  return m_generalParameters.get();
}

const EmulsionSupportParameters *
EmulsionGeometryManager::supportParameters() const
{
  return m_supportParameters.get();
}

const NeutrinoDD::DistortedMaterialManager * 
EmulsionGeometryManager::distortedMatManager() const
{    
  return m_distortedMatManager.get();
}

EmulsionGeometryManager&
EmulsionGeometryManager::operator=(const EmulsionGeometryManager& right) {
  if (this != &right) {
    m_options = right.m_options;
    m_athenaComps = right.m_athenaComps;
    m_commonItems = right.m_commonItems;
    m_rdb = right.m_rdb;
    m_filmParameters.reset(new EmulsionFilmParameters(m_rdb));
    m_platesParameters.reset(new EmulsionPlatesParameters(m_rdb));
    m_generalParameters.reset(new EmulsionGeneralParameters(m_rdb));
    m_supportParameters.reset(new EmulsionSupportParameters(m_rdb));
    m_distortedMatManager.reset(new NeutrinoDD::DistortedMaterialManager());
  }
  return *this;
}

EmulsionGeometryManager::EmulsionGeometryManager(const EmulsionGeometryManager& right) {
  m_options = right.m_options;
  m_athenaComps = right.m_athenaComps;
  m_commonItems = right.m_commonItems;
  m_rdb = right.m_rdb;
  m_filmParameters.reset(new EmulsionFilmParameters(m_rdb));
  m_platesParameters.reset(new EmulsionPlatesParameters(m_rdb));
  m_generalParameters.reset(new EmulsionGeneralParameters(m_rdb));
  m_supportParameters.reset(new EmulsionSupportParameters(m_rdb));
  m_distortedMatManager.reset(new NeutrinoDD::DistortedMaterialManager());
}
