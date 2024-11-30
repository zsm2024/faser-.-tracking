/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserByteStreamCnvSvc/FaserByteStreamCnvSvc.h"
//#include "ByteStreamCnvSvc/ByteStreamOutputSvc.h"
//#include "ByteStreamCnvSvcBase/FullEventAssembler.h"
//#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"

#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/TrigDecision.h"

//#include "eformat/SourceIdentifier.h"
//#include "eformat/StreamTag.h"

#include "GaudiKernel/IClassIDSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include <algorithm>

/// Standard constructor
FaserByteStreamCnvSvc::FaserByteStreamCnvSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : FaserByteStreamCnvSvcBase(name, pSvcLocator),
    m_evtStore ("StoreGateSvc", name)
{
  declareProperty("ByteStreamOutputSvc",     m_ioSvcName);
  declareProperty("ByteStreamOutputSvcList", m_ioSvcNameList);
  declareProperty("IsSimulation",  m_isSimulation = false);
  declareProperty("IsTestbeam",    m_isTestbeam   = false);
  declareProperty("IsCalibration", m_isCalibration= false);
  declareProperty("GetDetectorMask", m_getDetectorMask = false);
  declareProperty("UserType",      m_userType     = "RawEvent");
  declareProperty("EventStore",    m_evtStore);
}

/// Standard Destructor
FaserByteStreamCnvSvc::~FaserByteStreamCnvSvc() {
}

/// Initialize the service.
StatusCode FaserByteStreamCnvSvc::initialize() {
   if (!FaserByteStreamCnvSvcBase::initialize().isSuccess()) {
      ATH_MSG_FATAL("FaserByteStreamCnvSvcBase::initialize() failed");
      return(StatusCode::FAILURE);
   }

   ATH_CHECK( m_evtStore.retrieve() );

   // get ready for output
   std::vector<std::string> ioSvcNames = m_ioSvcNameList.value();
   if (!m_ioSvcName.empty()) {
      // add ioSvcName if ioSvcNameList is missing it
      std::vector<std::string>::iterator it = find(ioSvcNames.begin(), ioSvcNames.end(), m_ioSvcName);
      if (it == ioSvcNames.end()) {
         ioSvcNames.push_back(m_ioSvcName);
      }
   }
   
   if (ioSvcNames.size() != 0) {
      // Check FaserByteStreamCnvSvc
      for (std::vector<std::string>::iterator itSvc = ioSvcNames.begin(), itSvcE = ioSvcNames.end();
	      itSvc != itSvcE; ++itSvc) {
         ATH_MSG_DEBUG("get " << *itSvc);

	 ATH_MSG_WARNING("not doing anything with" << *itSvc);
	 /*
         // get service
         IService* svc;
         if (!service(*itSvc, svc).isSuccess()) {
            ATH_MSG_FATAL("Cannot get ByteStreamOutputSvc");
            return(StatusCode::FAILURE);
         }
         ByteStreamOutputSvc* ioSvc = dynamic_cast<ByteStreamOutputSvc*>(svc);
         if (ioSvc == 0) {
            ATH_MSG_FATAL("Cannot cast to  ByteStreamOutputSvc");
            return(StatusCode::FAILURE);
         }
         // get stream name
         std::string bsOutputStreamName;
         SimpleProperty<std::string> propBSO("BSOutputStreamName", bsOutputStreamName);
         if (!ioSvc->getProperty(&propBSO).isSuccess()) {
            ATH_MSG_FATAL("Cannot get BSOutputStreamName from " << *itSvc);
            return(StatusCode::FAILURE);
         }
         bsOutputStreamName = propBSO.value();
         // append
         m_ioSvcMap[bsOutputStreamName] = ioSvc;
	 */
      }
   }
   return(StatusCode::SUCCESS);
}

StatusCode FaserByteStreamCnvSvc::finalize() {
  return(FaserByteStreamCnvSvcBase::finalize());
}

StatusCode FaserByteStreamCnvSvc::connectOutput(const std::string& t, const std::string& /*mode*/) {
   return(connectOutput(t));
}

StatusCode FaserByteStreamCnvSvc::connectOutput(const std::string& /*t*/) {
   ATH_MSG_DEBUG("In connectOutput");

   /*
   // Get the EventInfo obj for run/event number
   const xAOD::EventInfo* evtInfo{nullptr};
   ATH_CHECK( m_evtStore->retrieve(evtInfo) );
   uint64_t event = evtInfo->eventNumber();
   uint32_t run_no = evtInfo->runNumber();
   uint32_t bc_time_sec = evtInfo->timeStamp();
   uint32_t bc_time_ns = evtInfo->timeStampNSOffset();
   uint32_t run_type = 0;
   uint32_t lvl1_id = evtInfo->extendedLevel1ID();
   if (lvl1_id == 0) {
      lvl1_id = event;
   }
   uint32_t lvl1_type = evtInfo->level1TriggerType();
   uint64_t global_id = event;
   uint16_t lumi_block = evtInfo->lumiBlock();
   uint16_t bc_id = evtInfo->bcid();
   static uint8_t nevt = 0;
   nevt = nevt%255;
   // create an empty RawEvent
   eformat::helper::SourceIdentifier sid = eformat::helper::SourceIdentifier(eformat::FULL_SD_EVENT, nevt);
   m_rawEventWrite = new RawEventWrite(sid.code(), bc_time_sec, bc_time_ns, global_id, run_type, run_no, lumi_block, lvl1_id, bc_id, lvl1_type);
   */
   return(StatusCode::SUCCESS);
}

StatusCode FaserByteStreamCnvSvc::commitOutput(const std::string& outputConnection, bool /*b*/) {
   ATH_MSG_DEBUG("In flushOutput " << outputConnection);

   if (m_ioSvcMap.size() == 0) {
      ATH_MSG_ERROR("FaserByteStreamCnvSvc not configure for output");
      return(StatusCode::FAILURE);
   }
   /*
   writeFEA();

   // Get EventInfo
   const xAOD::EventInfo* evtInfo{nullptr};
   ATH_CHECK( m_evtStore->retrieve(evtInfo) );

   // Try to get TrigDecision
   const xAOD::TrigDecision* trigDecision{nullptr};
   if(m_evtStore->retrieve(trigDecision)!=StatusCode::SUCCESS) {
     ATH_MSG_WARNING("Failed to retrieve xAOD::TrigDecision. Will write empty trigger decision vectors");
     trigDecision = nullptr;
   }

   // change trigger info in Header
   uint32_t *l1Buff{nullptr};
   uint32_t *l2Buff{nullptr};
   uint32_t *efBuff{nullptr};
   uint32_t *encTag{nullptr};

   m_rawEventWrite->lvl1_id(evtInfo->extendedLevel1ID());
   m_rawEventWrite->lvl1_trigger_type((uint8_t)(evtInfo->level1TriggerType()));

   // LVL1 info
   uint32_t l1Size{0};
   if(trigDecision) {
     const std::vector<uint32_t>& tbp = trigDecision->tbp();
     const std::vector<uint32_t>& tap = trigDecision->tap();
     const std::vector<uint32_t>& tav = trigDecision->tav();
     size_t l1TotSize = tbp.size()+tap.size()+tav.size();
     if(l1TotSize>0) {
       l1Buff = new uint32_t[l1TotSize];
       for(uint32_t tb : tbp) {
	 l1Buff[l1Size++] = tb;
       }
       for(uint32_t tb : tap) {
	 l1Buff[l1Size++] = tb;
       }
       for(uint32_t tb : tav) {
	 l1Buff[l1Size++] = tb;
       }
     }
   }
   m_rawEventWrite->lvl1_trigger_info(l1Size, l1Buff);

   // LVL2 info
   uint32_t l2Size{0};
   if(trigDecision) {
     const std::vector<uint32_t>& lvl2PP = trigDecision->lvl2PassedPhysics();
     if(lvl2PP.size()>0) {
       l2Buff = new uint32_t[lvl2PP.size()];
       for(uint32_t tb : lvl2PP) {
	 l2Buff[l2Size++] = tb;
       }
     }
   }
   m_rawEventWrite->lvl2_trigger_info(l2Size, l2Buff);

   // EF info
   uint32_t efSize{0};
   if(trigDecision) {
     const std::vector<uint32_t>& efPP = trigDecision->efPassedPhysics();
     if(efPP.size()>0) {
       efBuff = new uint32_t[efPP.size()];
       for(uint32_t tb : efPP) {
	 efBuff[efSize++] = tb;
       }
     }
   }
   m_rawEventWrite->event_filter_info(efSize, efBuff);

   // stream tag
   std::vector<eformat::helper::StreamTag> on_streamTags;
   const std::vector<xAOD::EventInfo::StreamTag>& off_streamTags = evtInfo->streamTags();
   for(const auto& sTag : off_streamTags) {
     // convert offline -> online
     eformat::helper::StreamTag tmpTag;
     tmpTag.name = sTag.name();
     tmpTag.type = sTag.type();
     tmpTag.obeys_lumiblock = sTag.obeysLumiblock();
     for(uint32_t rob : sTag.robs()) {
       tmpTag.robs.insert(rob);
     }
     for(uint32_t det : sTag.dets()) {
       tmpTag.dets.insert((eformat::SubDetector)det);
     }
     on_streamTags.push_back(tmpTag);
   }
   // encode
   uint32_t encSize = eformat::helper::size_word(on_streamTags);
   encTag = new uint32_t[encSize];
   eformat::helper::encode(on_streamTags, encSize, encTag);
   m_rawEventWrite->stream_tag(encSize, encTag);

   // convert RawEventWrite to RawEvent
   uint32_t rawSize = m_rawEventWrite->size_word();
   OFFLINE_FRAGMENTS_NAMESPACE::DataType* buffer = new OFFLINE_FRAGMENTS_NAMESPACE::DataType[rawSize];
   uint32_t count = eformat::write::copy(*(m_rawEventWrite->bind()), buffer, rawSize);
   if (count != rawSize) {
      ATH_MSG_ERROR("Memcopy failed");
      return(StatusCode::FAILURE);
   }
   RawEvent rawEvent(buffer);
   // check validity
   try {
      rawEvent.check_tree();
   } catch (...) {
      ATH_MSG_ERROR("commitOutput failed, because FullEventFragment invalid");
      return(StatusCode::FAILURE);
   }
   ATH_MSG_DEBUG("commitOutput: Size of Event (words) = " << rawEvent.fragment_size_word());
   // put event to OutputSvc
   if ((m_ioSvcMap.size() == 1) or (m_ioSvcMap.count(outputConnection) > 0)) {
      std::map<std::string, ByteStreamOutputSvc*>::iterator itSvc = m_ioSvcMap.find(outputConnection);
      // for backward compatibility
      if (itSvc == m_ioSvcMap.end()) {
         itSvc = m_ioSvcMap.begin();
      }
      // put
      if (!itSvc->second->putEvent(&rawEvent)) {
         ATH_MSG_ERROR("commitOutput failed to send output");
         return(StatusCode::FAILURE);
      }
   }
   // delete
   delete [] buffer; buffer = 0;
   delete m_rawEventWrite; m_rawEventWrite = 0;
   delete [] l1Buff; l1Buff = 0;
   delete [] l2Buff; l2Buff = 0;
   delete [] efBuff; efBuff = 0;
   delete [] encTag; encTag = 0;
   // delete FEA
   for (std::map<std::string, FullEventAssemblerBase*>::const_iterator it = m_feaMap.begin(),
	   itE = m_feaMap.end(); it != itE; it++) {
      delete it->second;
   }
   m_feaMap.clear();

*/
   return(StatusCode::SUCCESS);
}

/*
void FaserByteStreamCnvSvc::writeFEA() {
   ATH_MSG_DEBUG("before FEAMAP size = " << m_feaMap.size());
   for (std::map<std::string, FullEventAssemblerBase*>::const_iterator it = m_feaMap.begin(),
	   itE = m_feaMap.end(); it != itE; it++) {
      MsgStream log(msgSvc(), name());
      (*it).second->fill(m_rawEventWrite, log);
   }
   ATH_MSG_DEBUG("after FEAMAP size = " << m_feaMap.size());
}
*/
