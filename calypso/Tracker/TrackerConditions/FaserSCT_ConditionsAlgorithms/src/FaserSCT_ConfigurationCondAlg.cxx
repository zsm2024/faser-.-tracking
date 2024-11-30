/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_ConfigurationCondAlg.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

#include <memory>


FaserSCT_ConfigurationCondAlg::FaserSCT_ConfigurationCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
    ::AthReentrantAlgorithm(name, pSvcLocator) {}

StatusCode FaserSCT_ConfigurationCondAlg::initialize() {
    ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  // Read Cond Handle
  ATH_CHECK(m_readKey.initialize());

  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_ConfigurationCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<FaserSCT_ConfigurationCondData> writeHandle{m_writeKey, ctx};
  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid. "
                  << "In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<FaserSCT_ConfigurationCondData> writeCdo{std::make_unique<FaserSCT_ConfigurationCondData>()};
  // clear structures before filling
  writeCdo->clear();

  if (writeCdo == nullptr) {
    ATH_MSG_FATAL("Pointer of derived conditions object is null");
    return StatusCode::FAILURE;
  }

  // Get read handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey, ctx};
  const CondAttrListCollection* readCdo {*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of " << m_readKey.key() << " is " << readCdo->size());

  // Get the validity range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);

  writeHandle.addDependency(readHandle);

  std::string sensorParam{"sensor"};
  std::string nstripParam{"nstrips"};
  std::string noisyParam{"noisyList"};
  //std::string stripParam{"strip"};
  //std::string occupancyParam{"occupancy"};
  CondAttrListCollection::const_iterator itr{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesn't support C++11 type loops, no generic 'begin'
  for (; itr != end; ++itr) {
    // A CondAttrListCollection is a map of ChanNum and AttributeList
    CondAttrListCollection::ChanNum channelNumber{itr->first};
    const CondAttrListCollection::AttributeList &payload{itr->second};

    if (not (payload.exists(sensorParam) and not payload[sensorParam].isNull() and
	     payload.exists(nstripParam) and not payload[nstripParam].isNull() and
	     payload.exists(noisyParam) )) {
      ATH_MSG_FATAL(sensorParam << " and/or " << noisyParam << " does not exist.");
      continue;
    }

    auto sensorVal {payload[sensorParam].data<int>()};
    auto nstripVal {payload[nstripParam].data<int>()};
    auto channel {payload[sensorParam].data<int>()};

    if (channel != int(channelNumber)) {
      ATH_MSG_FATAL("Sensor " << payload[sensorParam] << " != channel " << channelNumber << "!");
      return StatusCode::FAILURE;
    }

    if (payload[noisyParam].isNull()) continue;  // No noisy strips

    std::string noisyListVal {payload[noisyParam].data<std::string>()};

    // Parse string of noisy strips
    std::stringstream ss(noisyListVal);
    std::string item;
    int nfound = 0;
    ATH_MSG_VERBOSE("Parsing noisy strips string: " << noisyListVal);

    while (ss >> item) {

      if(item.size() == 0) continue;

      //ATH_MSG_VERBOSE("["<<item<<"]");

      // Split by colon
      auto i = item.find(':');
      int stripVal(std::stoi(item.substr(0, i)));
      float occupancy(std::stof(item.substr((i+1), (i+7))));

      ATH_MSG_VERBOSE("Found sensor " << sensorVal << " strip " << stripVal << " occupancy " << occupancy);
      nfound++;

      Identifier waferId = m_idHelper->wafer_id(sensorVal);
      Identifier stripId = m_idHelper->strip_id(waferId, stripVal);
      if (stripId.is_valid() and occupancy > m_occupancyThreshold) {
	writeCdo->setBadStripId(stripId, sensorVal, stripVal);
	ATH_MSG_INFO("Bad Strip: " << m_idHelper->station(stripId) << "/" << m_idHelper->layer(stripId) << "/"
		     << m_idHelper->phi_module(stripId) << "/" << m_idHelper->phi_module(stripId) << "/"
		     << m_idHelper->side(stripId) << "/" << m_idHelper->strip(stripId) << " (" << sensorVal<< ") : "
		     << occupancy);
      } else {
	ATH_MSG_VERBOSE(m_idHelper->station(stripId) << "/" << m_idHelper->layer(stripId) << "/"
			<< m_idHelper->phi_module(stripId) << "/" << m_idHelper->phi_module(stripId) << "/"
			<< m_idHelper->side(stripId) << "/" << m_idHelper->strip(stripId) << " (" << sensorVal<< ") : "
			<< occupancy);
      }
    } // End of parsing line
      
    // Check that we found the strips we should have
    if (nfound != nstripVal) {
      ATH_MSG_FATAL("Sensor " << sensorVal << " found " << nfound << " but expected " << nstripVal);
      return StatusCode::FAILURE;
    }

  } // End of loop over sensors

  // Summarize what we found
  ATH_MSG_INFO("From " << readCdo->size() << " sensors in DB");
  ATH_MSG_INFO("found " << writeCdo->getBadStripIds()->size() << " bad strips");
  ATH_MSG_INFO("found " << writeCdo->getBadModuleIds()->size() << " bad modules");

  if (writeHandle.record(std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_ConfigurationCondData " << writeHandle.key()
                  << " with EventRange " << writeHandle.getRange()
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << writeHandle.getRange() << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_ConfigurationCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
