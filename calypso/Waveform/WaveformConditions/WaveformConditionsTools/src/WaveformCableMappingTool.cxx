/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FASER collaborations
*/

/** @file WaveformCableMappingTool.cxx Implementation file for WaveformCableMappingTool.
    @author Eric Torrence (05/02/22)
    Based on FaserSCT_CableMappingTool
*/

#include "WaveformCableMappingTool.h"

//----------------------------------------------------------------------
WaveformCableMappingTool::WaveformCableMappingTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
WaveformCableMappingTool::initialize() {
  // Read Cond Handle Key

  ATH_MSG_DEBUG("WaveformCableMappingTool::initialize()");

  ATH_CHECK(m_readKey.initialize());

  // Set up helpers
  ATH_CHECK(detStore()->retrieve(m_ecalID, "EcalID"));
  ATH_CHECK(detStore()->retrieve(m_vetoID, "VetoID"));
  ATH_CHECK(detStore()->retrieve(m_vetoNuID, "VetoNuID"));
  ATH_CHECK(detStore()->retrieve(m_triggerID, "TriggerID"));
  ATH_CHECK(detStore()->retrieve(m_preshowerID, "PreshowerID"));

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
StatusCode
WaveformCableMappingTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------
WaveformCableMap
WaveformCableMappingTool::getCableMapping(const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getCableMapping()");
  WaveformCableMap mappingData;

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
  /*
  std::string typeParam{"type"};
  std::string stationParam{"station"};
  std::string plateParam{"plate"};
  std::string rowParam{"row"};
  std::string moduleParam{"module"};
  std::string pmtParam{"pmt"};
  */
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    const CondAttrListCollection::AttributeList &payload{attrList->second};
    if (payload.exists("type") and not payload["type"].isNull()) {

      std::string det_type{payload["type"].data<std::string>()};

      int stationVal{payload["station"].data<int>()};
      int plateVal  {payload["plate"].data<int>()};
      int rowVal    {payload["row"].data<int>()};
      int moduleVal {payload["module"].data<int>()};
      int pmtVal    {payload["pmt"].data<int>()};
      Identifier identifier;

      // Ugh, cant use switch statement with strings
      // Must do this using an if ladder
      if (det_type == "calo") {
	// Do checks of PMT identifier
	identifier = m_ecalID->pmt_id(rowVal, moduleVal, pmtVal, true);
	// Test a few things
	ATH_MSG_DEBUG("Calo  ID:" << identifier);
	ATH_MSG_DEBUG("PMT:" << m_ecalID->pmt(identifier) << " PMT Max:" << m_ecalID->pmt_max(identifier));
      }
      else if (det_type == "calo2") {
	// Do checks of PMT identifier
	identifier = m_ecalID->pmt_id(rowVal, moduleVal, pmtVal, true);
	ATH_MSG_DEBUG("Calo2 ID:" << identifier);
	ATH_MSG_DEBUG("PMT:" << m_ecalID->pmt(identifier) << " PMT Max:" << m_ecalID->pmt_max(identifier));
      }
      else if (det_type == "veto") {
	identifier = m_vetoID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "vetonu") {
	identifier = m_vetoNuID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "trigger") {
	identifier = m_triggerID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "preshower") {
	identifier = m_preshowerID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "clock") {
	// No valid identifiers for these
	identifier = -1;
      }
      else if (det_type == "none") {
	identifier = -1;
      }
      else {
	ATH_MSG_WARNING("Detector type " << det_type << " not known for channel " << channelNumber << "!");
	det_type = std::string("none");
	identifier = -1;
      }

      ATH_MSG_DEBUG("Mapped digitizer channel " << channelNumber << " to " << det_type<< " ID: " << identifier);

      mappingData.emplace(std::make_pair(channelNumber, std::make_pair(det_type, identifier)));

    }

  } // End of loop over attributes


  return mappingData;
} 

WaveformCableMap
WaveformCableMappingTool::getCableMapping(void) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getCableMapping(ctx);
}

//----------------------------------------------------------------------
int
WaveformCableMappingTool::getChannelMapping(const EventContext& ctx, const Identifier id) const {
  // Print where you are
  ATH_MSG_DEBUG("in getChannelMapping("<< id <<")");
  int channel = -1;

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return channel;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return channel;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);
  
  // Read mapping info
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; attrList!=end; ++attrList) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{attrList->first};
    const CondAttrListCollection::AttributeList &payload{attrList->second};
    if (payload.exists("type") and not payload["type"].isNull()) {

      std::string det_type{payload["type"].data<std::string>()};

      int stationVal{payload["station"].data<int>()};
      int plateVal  {payload["plate"].data<int>()};
      int rowVal    {payload["row"].data<int>()};
      int moduleVal {payload["module"].data<int>()};
      int pmtVal    {payload["pmt"].data<int>()};
      Identifier identifier;

      // Ugh, cant use switch statement with strings
      // Must do this using an if ladder
      if (det_type == "calo") {
	identifier = m_ecalID->pmt_id(rowVal, moduleVal, pmtVal);
      }
      else if (det_type == "calo2") {
	identifier = m_ecalID->pmt_id(rowVal, moduleVal, pmtVal);
      }
      else if (det_type == "veto") {
	identifier = m_vetoID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "vetonu") {
	identifier = m_vetoNuID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "trigger") {
	identifier = m_triggerID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "preshower") {
	identifier = m_preshowerID->pmt_id(stationVal, plateVal, pmtVal);
      }
      else if (det_type == "clock") {
	// No valid identifiers for these
	identifier = -1;
      }
      else if (det_type == "none") {
	identifier = -1;
      }
      else {
	ATH_MSG_WARNING("Detector type " << det_type << " not known for channel " << channelNumber << "!");
	det_type = std::string("none");
	identifier = -1;
      }

      // ATH_MSG_DEBUG("Comparing " << det_type << " ID: " 
      //	    << identifier.get_identifier()  
      //	    << " to requested " << id);

      // Is this the identifier we are looking for?
      if (id != identifier) continue;

      ATH_MSG_DEBUG("Mapped identifier " << det_type << " ID: " << identifier << " to digitizer channel " << channelNumber);

      channel = channelNumber;
      break;
    }

  } // End of loop over attributes

  if (channel < 0) {
    ATH_MSG_WARNING("No channel found for identifier " << id << "!");

  }

  return channel;
} 

int
WaveformCableMappingTool::getChannelMapping(const Identifier id) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getChannelMapping(ctx, id);
}

