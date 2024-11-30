/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoByteStreamAuxCnv.h"
#include "FaserByteStreamCnvSvc/FaserByteStreamCnvSvc.h"
//#include "FaserByteStreamCnvSvc/FaserByteStreamInputSvc.h"

#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"
#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"

/*
#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ByteStreamMetadata.h"
#include "ByteStreamData/ByteStreamMetadataContainer.h"
*/

#include "EventFormats/DAQFormats.hpp"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "StoreGate/StoreGateSvc.h"

//#include "eformat/StreamTag.h"

#include <time.h>

using DAQFormats::EventFull;

EventInfoByteStreamAuxCnv::EventInfoByteStreamAuxCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , AthMessaging(svcloc != nullptr ? msgSvc() : nullptr, "EventInfoByteStreamAuxCnv")
  , m_ByteStreamCnvSvc(nullptr)
  , m_robDataProvider("FaserROBDataProviderSvc", "EventInfoByteStreamAuxCnv")
    //  , m_mdSvc("InputMetaDataStore", "EventInfoByteStreamAuxCnv")
  , m_isSimulation(false)
  , m_isTestbeam(false)
  , m_isCalibration(false)
{
}

const CLID& EventInfoByteStreamAuxCnv::classID() 
{
  return ClassID_traits<xAOD::EventAuxInfo>::ID();
}

long EventInfoByteStreamAuxCnv::storageType() 
{
  return FaserByteStreamAddress::storageType();
}

StatusCode EventInfoByteStreamAuxCnv::initialize() 
{
  ATH_MSG_DEBUG("EventInfoByteStreamAuxCnv::Initialize()");

  CHECK(Converter::initialize());

  // Check ByteStreamCnvSvc
  ATH_MSG_DEBUG("Find FaserByteStreamCnvSvc");
  IService* svc{nullptr};
  StatusCode sc = serviceLocator()->getService("FaserByteStreamCnvSvc", svc);
  if (!sc.isSuccess()) {
    ATH_MSG_ERROR("Cannot get FaserByteStreamCnvSvc ");
    return sc;
  } else {
    ATH_MSG_DEBUG("Located FaserByteStreamCnvSvc");
  }

  // Don't need this as we no longer have an interface class, but OK
  m_ByteStreamCnvSvc = dynamic_cast<FaserByteStreamCnvSvc*>(svc);
  if (!m_ByteStreamCnvSvc) {
    ATH_MSG_ERROR("Cannot cast to FaserByteStreamCnvSvc");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Find ROBDataProvider");
  CHECK(m_robDataProvider.retrieve());
  //CHECK(m_mdSvc.retrieve());

  SimpleProperty<bool> propIsSimulation("IsSimulation", m_isSimulation);
  sc = m_ByteStreamCnvSvc->getProperty(&propIsSimulation);
  if (sc.isSuccess()) {
    m_isSimulation = propIsSimulation.value();
    ATH_MSG_INFO("IsSimulation : " << m_isSimulation);
  } 
  else {
    ATH_MSG_ERROR("Cannot get IsSimulation");
    return sc;
  }

  SimpleProperty<bool> propIsTestbeam("IsTestbeam", m_isTestbeam);
  sc = m_ByteStreamCnvSvc->getProperty(&propIsTestbeam);
  if (sc.isSuccess()) {
    m_isTestbeam = propIsTestbeam.value();
    ATH_MSG_INFO("IsTestbeam : " << m_isTestbeam);
  } 
  else {
    ATH_MSG_ERROR("Cannot get IsTestbeam");
    return sc;
  }

  SimpleProperty<bool> propIsCalibration("IsCalibration", m_isCalibration);
  sc = m_ByteStreamCnvSvc->getProperty(&propIsCalibration);
  if (sc.isSuccess()) {
    m_isCalibration = propIsCalibration.value();
    ATH_MSG_INFO("IsCalibration : " << m_isCalibration);
  } 
  else {
    ATH_MSG_ERROR("Cannot get IsCalibration");
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode EventInfoByteStreamAuxCnv::finalize() 
{
  ATH_MSG_DEBUG("Finalize");
  
  StatusCode sc = Converter::finalize();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Converter::finalize() failed");
  }
  return sc;
}

StatusCode EventInfoByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{

  ATH_MSG_DEBUG("EventInfoByteStreamAuxCnv::createObj() called");

  FaserByteStreamAddress *pRE_Addr{nullptr};
  pRE_Addr = dynamic_cast<FaserByteStreamAddress*>(pAddr);
  if (!pRE_Addr) {
    ATH_MSG_ERROR("Cannot cast to FaserByteStreamAddress ");
    return StatusCode::FAILURE;
  }


  // get RawEvent
  const EventFull* re = reinterpret_cast<const EventFull*>(m_robDataProvider->getEvent());

  if (!re) {
    ATH_MSG_ERROR("Can not get RawEvent ");
    return StatusCode::FAILURE;
  }

  // Run Number
  int runNumber = re->run_number();

  // Event Number (ordinal number of event in file)
  uint64_t eventNumber = re->event_counter();
  
  // Time Stamp
  uint32_t bc_time_sec = re->timestamp()/1E6;  // timestamp is in usec
  // Subtract off integer second and provide remainder in ns
  uint32_t bc_time_ns  = 1E3 * (re->timestamp() - 1E6 * bc_time_sec);
  // bc_time_ns should be lt 1e9.
  if (bc_time_ns > 1000000000) {
      // For later runs, the nanosecond clock sometimes is not reset, making it overrun 1e9. Round it off to 1e9
      ATH_MSG_WARNING("bc_time nanosecond number larger than 1e9, it is " << bc_time_ns << ", reset it to 1 sec");
      bc_time_ns = 1000000000;
  }

  // luminosity block number
  uint16_t lumiBlock = 0;

  // bunch crossing identifier
  uint16_t bcID = re->bc_id();

  // Unused
  unsigned int detMask0 = 0xFFFFFFFF, 
    detMask1 = 0xFFFFFFFF, 
    detMask2 = 0xFFFFFFFF, 
    detMask3 = 0xFFFFFFFF;

  xAOD::EventInfo evtInfo;
  xAOD::EventAuxInfo* pEvtInfoAux = new xAOD::EventAuxInfo();
  evtInfo.setStore(pEvtInfoAux);

  evtInfo.setRunNumber(runNumber);
  evtInfo.setEventNumber(eventNumber);
  evtInfo.setLumiBlock(lumiBlock);
  evtInfo.setTimeStamp(bc_time_sec);
  evtInfo.setTimeStampNSOffset(bc_time_ns);
  evtInfo.setBCID(bcID);
  evtInfo.setDetectorMask(detMask0,detMask1);
  evtInfo.setDetectorMaskExt(detMask2,detMask3);

  // The following values were implicitly set by the BS converter of the legacy EventInfo
  // Setting them here too
  evtInfo.setMCChannelNumber(0);
  evtInfo.setMCEventNumber(0);
  evtInfo.setMCEventWeights(std::vector<float>(1,1));

  // Set Event Type
  uint32_t eventTypeBitmask{0};
  if (m_isSimulation) {
    eventTypeBitmask |= xAOD::EventInfo::IS_SIMULATION;
  }
  if (m_isTestbeam) {
    eventTypeBitmask |= xAOD::EventInfo::IS_TESTBEAM;
  }
  if (m_isCalibration) {
    eventTypeBitmask |= xAOD::EventInfo::IS_CALIBRATION;
  }
  evtInfo.setEventTypeBitmask(eventTypeBitmask);

  /*
  // Trigger Info
  const OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer;
  // status element
  re->status(buffer);
  uint32_t statusElement = *buffer;

  // extended LVL1ID
  uint32_t extendedLevel1ID = re->lvl1_id();

  // LVL1 trigger type
  uint32_t level1TriggerType = re->lvl1_trigger_type();
  */

  // stream tag
  /*
  std::vector<xAOD::EventInfo::StreamTag> streamTags;
  std::vector<eformat::helper::StreamTag> onl_streamTags;
  re->stream_tag(buffer);
  eformat::helper::decode(re->nstream_tag(), buffer, onl_streamTags);
  for (const eformat::helper::StreamTag& onl_streamTag : onl_streamTags) {
    std::set<uint32_t> tmp_off_dets = std::set<uint32_t>();
    if (!onl_streamTag.dets.empty()) {
      std::set<eformat::SubDetector> tmp_onl_dets = onl_streamTag.dets;
      for (const eformat::SubDetector& subdet : tmp_onl_dets) {
        tmp_off_dets.insert((uint32_t) subdet);
      }
    }
    streamTags.push_back(xAOD::EventInfo::StreamTag(onl_streamTag.name
						    , onl_streamTag.type
						    , onl_streamTag.obeys_lumiblock
						    , onl_streamTag.robs
						    , tmp_off_dets)
			 );
  }

  evtInfo.setStatusElement(statusElement);
  evtInfo.setExtendedLevel1ID(extendedLevel1ID);
  evtInfo.setLevel1TriggerType(level1TriggerType);
  evtInfo.setStreamTags(streamTags);
  */

  // record EventInfo
  evtInfo.setEventFlags(xAOD::EventInfo::Core, m_robDataProvider->getEventStatus());
  pObj = SG::asStorable(pEvtInfoAux);

  ATH_MSG_DEBUG(" New xAOD::EventAuxInfo made, run/event= " << runNumber 
		<< "/" << eventNumber
		<< "  Time stamp = " << ascTime(bc_time_sec) 
		);

  return StatusCode::SUCCESS;
    
}

StatusCode EventInfoByteStreamAuxCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) 
{
  ATH_MSG_DEBUG("Nothing to be done for xAOD::EventAuxInfo createReps");
  return StatusCode::SUCCESS;
}

const char* EventInfoByteStreamAuxCnv::ascTime(unsigned int tstamp) 
{
  struct tm t;
  t.tm_sec   = tstamp;
  t.tm_min   = 0;
  t.tm_hour  = 0;
  t.tm_mday  = 0;
  t.tm_mon   = 0;
  t.tm_year  = 70;
  t.tm_wday  = 00;
  t.tm_yday  = 00;
  t.tm_isdst = 0;
  time_t ct = mktime(&t);
  tm* t2 = gmtime(&ct);
  return(asctime(t2));
}
