/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FASER collaborations
*/

/** @file WaveformRangeTool.cxx Implementation file for WaveformRangeTool.
    @author Eric Torrence (05/02/22)
*/

#include "WaveformRangeTool.h"

//----------------------------------------------------------------------
WaveformRangeTool::WaveformRangeTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
WaveformRangeTool::initialize() {
  // Read Cond Handle Key

  ATH_MSG_DEBUG("WaveformRangeTool::initialize()");

  ATH_CHECK(m_readKey.initialize());

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
StatusCode
WaveformRangeTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
WaveformRangeMap
WaveformRangeTool::getRangeMapping(const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getRangeMapping()");
  WaveformRangeMap mappingData;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return mappingData;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return mappingData;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);
  
  // Read range info

  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    const CondAttrListCollection::AttributeList &payload{attrList->second};
    if (payload.exists("range") and not payload["range"].isNull()) {

      float range {payload["range"].data<float>()};

      ATH_MSG_DEBUG("Found digitizer channel " << channelNumber << " range as " << range);

      mappingData.emplace(std::make_pair(channelNumber, range));

    }

  } // End of loop over attributes

  return mappingData;
} 

WaveformRangeMap
WaveformRangeTool::getRangeMapping(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getRangeMapping(ctx);
}



