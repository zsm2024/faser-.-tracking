/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_SiliconConditionsTool.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

// Constructor
FaserSCT_SiliconConditionsTool::FaserSCT_SiliconConditionsTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent)
{
  // These will get overwritten if used but give them some initial value anyway.
  m_geoModelTemperature      = m_defaultTemperature;
  m_geoModelBiasVoltage      = m_defaultBiasVoltage;
  m_geoModelDepletionVoltage = m_defaultDepletionVoltage;
}

// Initialize
StatusCode FaserSCT_SiliconConditionsTool::initialize() {
  ATH_MSG_DEBUG("FaserSCT_SiliconConditionsTool::initialize()");

  if (m_checkGeoModel or m_forceUseGeoModel) {
    m_useGeoModel = setConditionsFromGeoModel();
    if (m_useGeoModel) {
      ATH_MSG_INFO("Default conditions come from GeoModel.");
    } else {
      ATH_MSG_INFO("GeoModel requests to use Conditions DB.");
    }
  }

  ATH_CHECK(m_condKeyHV.initialize((not m_useGeoModel) and m_useDB));
  ATH_CHECK(m_condKeyTemp.initialize((not m_useGeoModel) and m_useDB));

  if (not m_useGeoModel) {
    ATH_MSG_INFO("Will use temperature and voltages from this service (not from GeoModel).");
   
    // Get from Conditions database. Register callback, etc.
    if (m_useDB) {
      ATH_CHECK(detStore()->retrieve(m_sct_id, "FaserSCT_ID"));

      ATH_MSG_INFO("SCTDCSSvc retrieved");
    }
  } else {
    // Otherwise we use the GeoModel values
    m_defaultTemperature      = m_geoModelTemperature;
    m_defaultBiasVoltage      = m_geoModelBiasVoltage;
    m_defaultDepletionVoltage = m_geoModelDepletionVoltage;
  } 
  return StatusCode::SUCCESS; 
}

// Finalize
StatusCode FaserSCT_SiliconConditionsTool::finalize() {
  ATH_MSG_DEBUG("FaserSCT_SiliconConditionsTool::finalize()");
  return StatusCode::SUCCESS; 
} 

// Silicon temperature (by Identifier)
float FaserSCT_SiliconConditionsTool::temperature(const Identifier& elementId, const EventContext& ctx) const {
  const IdentifierHash elementHash{m_sct_id->wafer_hash(elementId)};
  return temperature(elementHash, ctx);
}

// Silicon bias voltage (by Identifier)
float FaserSCT_SiliconConditionsTool::biasVoltage(const Identifier& elementId, const EventContext& ctx) const {
  const IdentifierHash elementHash{m_sct_id->wafer_hash(elementId)};
  return biasVoltage(elementHash, ctx);
}

// Silicon depletion voltage (by Identifier)
float FaserSCT_SiliconConditionsTool::depletionVoltage(const Identifier& /*elementId*/, const EventContext& /*ctx*/) const {
  return m_defaultDepletionVoltage;
}

// Silicon temperature (by IdentifierHash)
float FaserSCT_SiliconConditionsTool::temperature(const IdentifierHash& elementHash, const EventContext& ctx) const {
  if (m_useDB and (not m_useGeoModel)) {
    const FaserSCT_DCSFloatCondData* data{getCondDataTemp(ctx)};
    if (data==nullptr) return m_defaultTemperature;
    float temperature{m_defaultTemperature};
    if (not data->getValue(elementHash, temperature)) return m_defaultTemperature;
    if (temperature <= -30.){
      ATH_MSG_DEBUG( "Sensor temperature: "<< temperature <<" <  -30 ");
      return m_defaultTemperature; 
    }
    return temperature;
  }
  return m_defaultTemperature;
}

// Silicon bias voltage (by IdentifierHash)
float FaserSCT_SiliconConditionsTool::biasVoltage(const IdentifierHash& elementHash, const EventContext& ctx) const {

  if (m_useDB and (not m_useGeoModel)) {
    const FaserSCT_DCSFloatCondData* data{getCondDataHV(ctx)};
    if (data==nullptr) return m_defaultBiasVoltage;
    float hv{m_defaultBiasVoltage};
    if (not data->getValue(elementHash, hv)) return m_defaultBiasVoltage;
    if (hv < 0.) {
      ATH_MSG_DEBUG("HV: "<< hv <<" <  0 ");
      return m_defaultBiasVoltage;
    }
    return hv;
  }
  return m_defaultBiasVoltage;
}

// Silicon deplition voltage (by IdentifierHash)
float FaserSCT_SiliconConditionsTool::depletionVoltage(const IdentifierHash& /*elementHash*/, const EventContext& /*ctx*/) const {
  return m_defaultDepletionVoltage;
}

bool 
FaserSCT_SiliconConditionsTool::setConditionsFromGeoModel()
{
  bool conditionsPresent{false};
  bool useCondDB{false};
   
  if (m_rdbSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not locate RDBAccessSvc");
    return false;
  }

  if (m_geoModelSvc.retrieve().isFailure()) {
    ATH_MSG_ERROR("Could not locate GeoModelSvc");
    return false;
  }
  DecodeFaserVersionKey versionKey{&*m_geoModelSvc, "SCT"};
  ATH_MSG_DEBUG("Checking GeoModel Version Tag: "<<  versionKey.tag() << " at Node: " << versionKey.node());

  IRDBRecordset_ptr sctConditionsSet{m_rdbSvc->getRecordsetPtr("SctConditions", versionKey.tag(), versionKey.node(), "FASERDD")};
  if (sctConditionsSet->size()) {
    ATH_MSG_DEBUG("Default conditions available from GeoModel.");
    const IRDBRecord* defaultConditions{(*sctConditionsSet)[0]};
    m_geoModelTemperature = defaultConditions->getDouble("TEMPERATURE");
    m_geoModelBiasVoltage = defaultConditions->getDouble("BIASVOLT");
    m_geoModelDepletionVoltage = defaultConditions->getDouble("DEPLETIONVOLT");
    conditionsPresent = true;
    // If m_forceUseGeoModel set then we ignore the USECONDDB column and
    // keep useCondDB = false
    if ((not m_forceUseGeoModel) and (not defaultConditions->isFieldNull("USECONDDB"))) { // USECONDDB Not present in old SCT tables
      useCondDB = (defaultConditions->getInt("USECONDDB"));
    }
  } else {
    ATH_MSG_WARNING("Default conditions NOT available in GeoModel database. Using old GeoModel defaults");
    // These are pre DC3 geometries. Probably never will be used.
    m_geoModelTemperature = -7;
    m_geoModelBiasVoltage = 100;
    m_geoModelDepletionVoltage = 20; 
    conditionsPresent = true; 
  }

  return ((not useCondDB) and conditionsPresent);

}

const FaserSCT_DCSFloatCondData* FaserSCT_SiliconConditionsTool::getCondDataHV(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_DCSFloatCondData> condData{m_condKeyHV, ctx};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyHV.key());
    return nullptr;
  }
  return *condData;
}

const FaserSCT_DCSFloatCondData* FaserSCT_SiliconConditionsTool::getCondDataTemp(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_DCSFloatCondData> condData{m_condKeyTemp, ctx};
  if (not condData.isValid()) {
    ATH_MSG_ERROR("Failed to get " << m_condKeyTemp.key());
    return nullptr;
  }
  return *condData;
}
