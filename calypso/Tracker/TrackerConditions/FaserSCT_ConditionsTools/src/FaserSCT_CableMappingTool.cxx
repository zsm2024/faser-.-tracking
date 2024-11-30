/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FASER collaborations
*/

/** @file FaserSCT_CableMappingTool.cxx Implementation file for FaserSCT_CableMappingTool.
    @author Dave Casper (06/01/22)
*/

#include "FaserSCT_CableMappingTool.h"

// Include Athena stuff

//----------------------------------------------------------------------
FaserSCT_CableMappingTool::FaserSCT_CableMappingTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
FaserSCT_CableMappingTool::initialize() {
  // Read Cond Handle Key

  ATH_CHECK(m_readKey.initialize());

  return StatusCode::SUCCESS;
} // FaserSCT_CableMappingTool::initialize()

//----------------------------------------------------------------------
StatusCode
FaserSCT_CableMappingTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
} // FaserSCT_CableMappingTool::finalize()

//----------------------------------------------------------------------
std::map<int, std::pair<int, int> >
FaserSCT_CableMappingTool::getCableMapping(const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getCableMapping()");
  std::map<int, std::pair<int, int> > mappingData;

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
  
  // Read mapping info
  std::string stationParam{"station"};
  std::string planeParam{"plane"};
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    const CondAttrListCollection::AttributeList &payload{attrList->second};
    if (payload.exists(stationParam) and not payload[stationParam].isNull() and 
        payload.exists(planeParam) and not payload[planeParam].isNull()) 
    {
      int stationVal{payload[stationParam].data<int>()};
      int planeVal  {payload[planeParam].data<int>()};
      if (stationVal < 0 || planeVal < 0) continue;  // Don't include invalid entries
      mappingData.emplace(std::make_pair(channelNumber, std::make_pair(stationVal, planeVal)));
    } else {
      ATH_MSG_WARNING(stationParam << " and/or " << planeParam << " does not exist for ChanNum " << channelNumber);
    }
  }

  return mappingData;
} //FaserSCT_ReadCalibChipDataTool::getNPtGainData()

std::map<int, std::pair<int, int> >
FaserSCT_CableMappingTool::getCableMapping(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getCableMapping(ctx);
}

