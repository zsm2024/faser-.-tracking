/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FASER collaborations
*/

/** @file WaveformDigiConditionsTool.cxx Implementation file for WaveformDigiConditionsTool.
    @author Eric Torrence (04/05/22)
*/

#include "WaveformDigiConditionsTool.h"

//----------------------------------------------------------------------
WaveformDigiConditionsTool::WaveformDigiConditionsTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
WaveformDigiConditionsTool::initialize() {
  // Read Cond Handle Key

  ATH_MSG_DEBUG("WaveformDigiConditionsTool::initialize()");

  ATH_CHECK(m_digiReadKey.initialize());

  // Deal with COOL channel
  if (m_detector == "Calo") {
    m_cool_channel = COOL_Calo;
  }
  else if (m_detector == "VetoNu") {
    m_cool_channel = COOL_VetoNu;
  } 
  else if (m_detector == "Veto") {
    m_cool_channel = COOL_Veto;
  } 
  else if (m_detector == "Trigger") {
    m_cool_channel = COOL_Timing;
  } 
  else if (m_detector == "Timing") {
    m_cool_channel = COOL_Timing;
  } 
  else if (m_detector == "Preshower") {
    m_cool_channel = COOL_Preshower;
  } 
  else {
    ATH_MSG_FATAL("Unknown detector: " << m_detector);
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
StatusCode
WaveformDigiConditionsTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
float
WaveformDigiConditionsTool::base_mean(const EventContext& ctx) const {
  return get_value(ctx, "base_mean");
}

float
WaveformDigiConditionsTool::base_rms(const EventContext& ctx) const {
  return get_value(ctx, "base_rms");
}

float
WaveformDigiConditionsTool::cb_mean(const EventContext& ctx) const {
  return get_value(ctx, "cb_mean");
}

float
WaveformDigiConditionsTool::cb_sigma(const EventContext& ctx) const {
  return get_value(ctx, "cb_sigma");
}

float
WaveformDigiConditionsTool::cb_norm(const EventContext& ctx) const {
  return get_value(ctx, "cb_norm");
}

float
WaveformDigiConditionsTool::cb_alpha(const EventContext& ctx) const {
  return get_value(ctx, "cb_alpha");
}

float
WaveformDigiConditionsTool::cb_n(const EventContext& ctx) const {
  return get_value(ctx, "cb_n");
}

float
WaveformDigiConditionsTool::preshower_time_slope(const EventContext& ctx) const {
  return 1/230.; // 23 cm/ns
}

float
WaveformDigiConditionsTool::trigger_time_slope(const EventContext& ctx) const {
  return 1/130.; // 13 cm/ns
}

//----------------------------------------------------------------------
float
WaveformDigiConditionsTool::base_mean(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return base_mean(ctx);
}

float
WaveformDigiConditionsTool::base_rms(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return base_rms(ctx);
}

float
WaveformDigiConditionsTool::cb_mean(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return cb_mean(ctx);
}

float
WaveformDigiConditionsTool::cb_sigma(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return cb_sigma(ctx);
}

float
WaveformDigiConditionsTool::cb_norm(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return cb_norm(ctx);
}

float
WaveformDigiConditionsTool::cb_alpha(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return cb_alpha(ctx);
}

float
WaveformDigiConditionsTool::cb_n(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return cb_n(ctx);
}

float
WaveformDigiConditionsTool::preshower_time_slope(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return preshower_time_slope(ctx);
}

float
WaveformDigiConditionsTool::trigger_time_slope(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return trigger_time_slope(ctx);
}

//----------------------------------------------------------------------
float 
WaveformDigiConditionsTool::get_value(const EventContext& ctx, std::string arg) const {

  float val = -1.;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_digiReadKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return val;
  }

  // Read value for specific channel
  const CondAttrListCollection::AttributeList& payload{readCdo->attributeList(m_cool_channel)};

  if (payload.exists(arg) and not payload[arg].isNull()) {
    val = payload[arg].data<float>();
    // ATH_MSG_DEBUG("Found digi COOL channel " << m_cool_channel << " " << arg << " as " << val);
  } else {
    ATH_MSG_WARNING("No valid " << arg << " value found for digi COOL channel "<< m_cool_channel<<"!");
  }

  return val;

}







