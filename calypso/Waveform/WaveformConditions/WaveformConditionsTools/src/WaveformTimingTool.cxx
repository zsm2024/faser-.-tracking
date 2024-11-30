/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FASER collaborations
*/

/** @file WaveformTimingTool.cxx Implementation file for WaveformTimingTool.
    @author Eric Torrence (04/05/22)
*/

#include "WaveformTimingTool.h"

//----------------------------------------------------------------------
WaveformTimingTool::WaveformTimingTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
WaveformTimingTool::initialize() {
  // Read Cond Handle Key

  ATH_MSG_DEBUG("WaveformTimingTool::initialize()");

  ATH_CHECK(m_timingReadKey.initialize());
  ATH_CHECK(m_offsetReadKey.initialize());

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
StatusCode
WaveformTimingTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
float 
WaveformTimingTool::nominalTriggerTime(const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in nominalTriggerTime()");

  float time=-1.;

  // Read Cond Handle
  SG::ReadCondHandle<AthenaAttributeList> readHandle{m_timingReadKey, ctx};
  const AthenaAttributeList* readCdo(*readHandle);

  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return time;
  }

  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return time;
  }
  ATH_MSG_DEBUG("Range of input is " << rangeW);
  
  // Read time info

  const CondAttrListCollection::AttributeList &payload{*readCdo};
  if (payload.exists("NominalTriggerTime") and not payload["NominalTriggerTime"].isNull()) {
    time = payload["NominalTriggerTime"].data<float>();
    ATH_MSG_DEBUG("Found nominal trigger time "<<time<<" ns");
  } else {
    ATH_MSG_WARNING("No valid nominal trigger time found!");
  }

  return time;
} 

//----------------------------------------------------------------------
float
WaveformTimingTool::triggerTimeOffset(const EventContext& ctx, int channel) const {

  ATH_MSG_DEBUG("in triggerTimeOffset("<<channel<<")");

  float time=0.;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_offsetReadKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return time;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return time;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);

  // Read offset for specific channel
  const CondAttrListCollection::AttributeList& payload{readCdo->attributeList(channel)};

  if (payload.exists("TriggerOffset") and not payload["TriggerOffset"].isNull()) {
    time = payload["TriggerOffset"].data<float>();
    ATH_MSG_DEBUG("Found digitizer channel " << channel << " triger offset as " << time);
  } else {
    ATH_MSG_WARNING("No valid trigger offset found for channel "<<channel<<"!");
  }

  return time;

}

//----------------------------------------------------------------------
float
WaveformTimingTool::nominalTriggerTime(void) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return nominalTriggerTime(ctx);
}

float
WaveformTimingTool::triggerTimeOffset(int channel) const {
  const EventContext& ctx(Gaudi::Hive::currentContext());
  return triggerTimeOffset(ctx, channel);
}






