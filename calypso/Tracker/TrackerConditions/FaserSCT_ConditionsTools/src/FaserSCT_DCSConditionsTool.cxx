/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// New FaserSCT_DCSConditions Tool, based on existing tool in SCT_ConditionsAlgs
// A. R-Veronneau 26/02/08

#include "FaserSCT_DCSConditionsTool.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "GaudiKernel/ThreadLocalContext.h"

#include "FaserSCT_IdConverter.h"
using FaserSCT_ConditionsTools::castId;

const Identifier FaserSCT_DCSConditionsTool::s_invalidId;
const float FaserSCT_DCSConditionsTool::s_defaultHV{-30.}; 
const float FaserSCT_DCSConditionsTool::s_defaultTemperature{-40.};

FaserSCT_DCSConditionsTool::FaserSCT_DCSConditionsTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{ 
}

StatusCode FaserSCT_DCSConditionsTool::initialize() {
  if (detStore()->retrieve(m_pHelper,"FaserSCT_ID").isFailure()) {
    ATH_MSG_ERROR("FaserSCT helper failed to retrieve ");
    return StatusCode::FAILURE;
  }
  
  // Read Cond Handle Keys
  if ((m_readAllDBFolders and m_returnHVTemp) or m_returnHVTemp) {
    ATH_CHECK(m_condKeyHV.initialize());
    ATH_CHECK(m_condKeyTemp0.initialize());
  }
  if ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)) {
    ATH_CHECK(m_condKeyState.initialize());
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_DCSConditionsTool::finalize() { 
  return StatusCode::SUCCESS;
}

//Can report about the module as a whole or the wafer
bool FaserSCT_DCSConditionsTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_MODULE or h==InDetConditions::SCT_SIDE or h==InDetConditions::SCT_STRIP);
}   

//returns the module ID (int), or returns -1 if not able to report
Identifier FaserSCT_DCSConditionsTool::getModuleID(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return s_invalidId;  

  Identifier moduleId;

  if (h==InDetConditions::SCT_MODULE) {
    moduleId = elementId;
  } else if (h==InDetConditions::SCT_SIDE) {
    moduleId = m_pHelper->module_id(elementId); 
  } else if (h==InDetConditions::SCT_STRIP) {
    Identifier waferId = m_pHelper->wafer_id(elementId);
    moduleId = m_pHelper->module_id(waferId); 
  }
  return moduleId;
}

//Returns if element Id is good or bad
bool FaserSCT_DCSConditionsTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  Identifier moduleId{getModuleID(elementId, h)};
  if (not moduleId.is_valid()) return true; // not canreportabout

  if ((m_readAllDBFolders and m_returnHVTemp) or (not m_readAllDBFolders and not m_returnHVTemp)) {
    const FaserSCT_DCSStatCondData* condDataState{getCondDataState(ctx)};
    if (!condDataState) return false; // no cond data
    else if (condDataState->output(castId(moduleId))==0) return true; //No params are listed as bad
    else return false;
  } else {
    return true;
  }
}

bool FaserSCT_DCSConditionsTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(elementId, ctx, h);
}

//Does the same for hashIds
bool FaserSCT_DCSConditionsTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier waferId{m_pHelper->wafer_id(hashId)};
  Identifier moduleId{m_pHelper->module_id(waferId)};
  return isGood(moduleId, ctx, InDetConditions::SCT_MODULE);
}

bool FaserSCT_DCSConditionsTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(hashId, ctx);
}

/////////////////////////////////// 

// some lame helper methods: 
// returns HV (s_defaultHV(-30) if there is no information)
float FaserSCT_DCSConditionsTool::modHV(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  Identifier moduleId{getModuleID(elementId, h)};
  if (not moduleId.is_valid()) return s_defaultHV; // not canreportabout, return s_defaultHV(-30)

  const FaserSCT_DCSFloatCondData* condDataHV{getCondDataHV(ctx)};
  if (!condDataHV) return s_defaultHV; // no cond data

  float hvval{s_defaultHV};
  if (condDataHV->getValue(castId(moduleId), hvval) and isGood(elementId, h)) {
    return hvval;
  }
  return s_defaultHV; //didn't find the module, return s_defaultHV(-30)
}

float FaserSCT_DCSConditionsTool::modHV(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return modHV(elementId, ctx, h);
}

//Does the same for hashIds
float FaserSCT_DCSConditionsTool::modHV(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier waferId{m_pHelper->wafer_id(hashId)};
  Identifier moduleId{m_pHelper->module_id(waferId)};
  return modHV(moduleId, ctx, InDetConditions::SCT_MODULE);
}

float FaserSCT_DCSConditionsTool::modHV(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return modHV(hashId, ctx);
}

//Returns temp0 (s_defaultTemperature(-40) if there is no information)
float FaserSCT_DCSConditionsTool::hybridTemperature(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  Identifier moduleId{getModuleID(elementId, h)};
  if (not moduleId.is_valid()) return s_defaultTemperature; // not canreportabout

  const FaserSCT_DCSFloatCondData* condDataTemp0{getCondDataTemp0(ctx)};
  if (!condDataTemp0) return s_defaultTemperature; // no cond data

  float temperature{s_defaultTemperature};
  if (condDataTemp0->getValue(castId(moduleId), temperature) and isGood(elementId, h)) {
    return temperature;
  }
  return s_defaultTemperature;//didn't find the module, return -40. 
}

float FaserSCT_DCSConditionsTool::hybridTemperature(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return hybridTemperature(elementId, ctx, h);
}

//Does the same for hashIds
float FaserSCT_DCSConditionsTool::hybridTemperature(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier waferId{m_pHelper->wafer_id(hashId)};
  Identifier moduleId{m_pHelper->module_id(waferId)};
  return hybridTemperature(moduleId, ctx, InDetConditions::SCT_MODULE);
}

float FaserSCT_DCSConditionsTool::hybridTemperature(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return hybridTemperature(hashId, ctx);
}

//Returns temp0 + correction for Lorentz angle calculation (s_defaultTemperature(-40) if there is no information)
float FaserSCT_DCSConditionsTool::sensorTemperature(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  Identifier moduleId{getModuleID(elementId, h)};
  if (not moduleId.is_valid()) return s_defaultTemperature; // not canreportabout

  const FaserSCT_DCSFloatCondData* condDataTemp0{getCondDataTemp0(ctx)};
  if (!condDataTemp0) return s_defaultTemperature; // no cond data

  float temperature{s_defaultTemperature};
  if (condDataTemp0->getValue(castId(moduleId), temperature) and isGood(elementId, h)) {
    // int bec{m_pHelper->station(moduleId)};
    // if (bec==0) { // Barrel
      return ( temperature + m_barrel_correction);  //return the temp+correction
    // } else { // Endcaps
    //   int modeta{m_pHelper->eta_module(moduleId)};
    //   if (modeta==2) {
    //     return (temperature + m_ecInner_correction);  //return the temp+correction
    //   } else {
    //     return (temperature + m_ecOuter_correction);  //return the temp+correction
    //   }
    // }
  }
  return s_defaultTemperature;  //didn't find the module, return s_defaultTemperature(-40).
} 

float FaserSCT_DCSConditionsTool::sensorTemperature(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return sensorTemperature(elementId, ctx, h);
}

//Does the same for hashIds
float FaserSCT_DCSConditionsTool::sensorTemperature(const IdentifierHash& hashId, const EventContext& ctx) const {
  Identifier waferId{m_pHelper->wafer_id(hashId)};
  Identifier moduleId{m_pHelper->module_id(waferId)};
  return sensorTemperature(moduleId, ctx, InDetConditions::SCT_MODULE);
}

float FaserSCT_DCSConditionsTool::sensorTemperature(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return sensorTemperature(hashId, ctx);
}

///////////////////////////////////

const FaserSCT_DCSStatCondData*
FaserSCT_DCSConditionsTool::getCondDataState(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_DCSStatCondData> condData{m_condKeyState, ctx};
  return condData.retrieve();
}

const FaserSCT_DCSFloatCondData*
FaserSCT_DCSConditionsTool::getCondDataHV(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_DCSFloatCondData> condData{m_condKeyHV, ctx};
  return condData.retrieve();
}

const FaserSCT_DCSFloatCondData*
FaserSCT_DCSConditionsTool::getCondDataTemp0(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_DCSFloatCondData> condData{m_condKeyTemp0, ctx};
  return condData.retrieve();
}
