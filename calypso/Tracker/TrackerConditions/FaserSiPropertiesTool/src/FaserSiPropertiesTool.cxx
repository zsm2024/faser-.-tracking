/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSiPropertiesTool.h"

FaserSiPropertiesTool::FaserSiPropertiesTool(const std::string& type, const std::string& name, const IInterface* parent) : 
  base_class(type, name, parent),
  m_propertiesVector{"SCTSiliconPropertiesVector"}
{
  declareProperty("DetectorName", m_detectorName="SCT", "Dectector name: SCT");
  declareProperty("ReadKey", m_propertiesVector, "Key of SiliconPropertiesVector");
}

StatusCode 
FaserSiPropertiesTool::initialize()
{ 
  ATH_MSG_INFO("FaserSiPropertiesTool Initialized");
  
  if (m_detectorName != "SCT") {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be SCT.");
    return StatusCode::FAILURE;
  }
  ATH_CHECK(m_propertiesVector.initialize());

  return StatusCode::SUCCESS;
}

StatusCode 
FaserSiPropertiesTool::finalize()
{
  return StatusCode::SUCCESS;
}

const Tracker::FaserSiliconProperties&
FaserSiPropertiesTool::getSiProperties(const IdentifierHash& elementHash, const EventContext& ctx) const {
  SG::ReadCondHandle<Tracker::FaserSiliconPropertiesVector> handle{m_propertiesVector, ctx};
  if (handle.isValid()) {
    const Tracker::FaserSiliconPropertiesVector* vector{*handle};
    if (vector) {
      return vector->getSiProperties(elementHash);
    }
  }

  ATH_MSG_WARNING("SG::ReadCondHandle<Tracker::FaserSiliconPropertiesVector> is not valid. " << 
                  "Return default Tracker::SiliconProperties for IdentifierHash of " <<
                  elementHash);
  return s_defaultProperties;
}

const Tracker::FaserSiliconProperties FaserSiPropertiesTool::s_defaultProperties{};
