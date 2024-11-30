/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSCT_ClusterContainerCnv_p1.h"

#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerCluster_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/TrackerPRD_Container_p1.h"
#include "TrackerEventTPCnv/TrackerPrepRawData/FaserSCT_ClusterCnv_p1.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"

// Athena
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/StoreGateSvc.h"

// Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"

void Tracker::FaserSCT_ClusterContainerCnv_p1::transToPers(const Tracker::FaserSCT_ClusterContainer* transCont,  Tracker::TrackerPRD_Container_p1* persCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all RDO
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and add their channels
    // to the container's vector, saving the indexes in the
    // collection. 

    typedef Tracker::FaserSCT_ClusterContainer TRANS;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

    FaserSCT_ClusterCnv_p1  chanCnv;
    TRANS::const_iterator it_Coll     = transCont->begin();
    TRANS::const_iterator it_CollEnd  = transCont->end();
    unsigned int collIndex;
    unsigned int chanBegin = 0;
    unsigned int chanEnd = 0;
    persCont->m_collections.resize(transCont->numberOfCollections());
//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Preparing " << persCont->m_collections.size() << "Collections" << endmsg;
  
    for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
        // Add in new collection
//         if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " New collection" << endmsg;
        const Tracker::FaserSCT_ClusterCollection& collection = (**it_Coll);
        chanBegin  = chanEnd;
        chanEnd   += collection.size();
        Tracker::TrackerPRD_Collection_p1& pcollection = persCont->m_collections[collIndex];
        pcollection.m_id    = collection.identify().get_compact();
        pcollection.m_hashId = (unsigned int) collection.identifyHash();
        pcollection.m_begin = chanBegin;
        pcollection.m_end   = chanEnd;
        // Add in channels
        persCont->m_PRD.resize(chanEnd);
        for (unsigned int i = 0; i < collection.size(); ++i) {
            const Tracker::FaserSCT_Cluster* chan = collection[i];
            persCont->m_PRD[i + chanBegin] = toPersistent((CONV**)0, chan, log );
        }
    }
//   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Writing SCT_ClusterContainer ***" << endmsg;
}

void  Tracker::FaserSCT_ClusterContainerCnv_p1::persToTrans(const Tracker::TrackerPRD_Container_p1* persCont, Tracker::FaserSCT_ClusterContainer* transCont, MsgStream &log) 
{

    // The transient model has a container holding collections and the
    // collections hold channels.
    //
    // The persistent model flattens this so that the persistent
    // container has two vectors:
    //   1) all collections, and
    //   2) all channels
    //
    // The persistent collections, then only maintain indexes into the
    // container's vector of all channels. 
    //
    // So here we loop over all collection and extract their channels
    // from the vector.

    const TrackerDD::SiDetectorElementCollection* elements(nullptr);
    if (m_useDetectorElement) {
        SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEleHandle(m_SCTDetEleCollKey);
        elements = *sctDetEleHandle;
        if (not sctDetEleHandle.isValid() or elements==nullptr) {
            log << MSG::FATAL << m_SCTDetEleCollKey.fullKey() << " is not available." << endmsg;
            return;
        }
    }

    Tracker::FaserSCT_ClusterCollection* coll = 0;

    FaserSCT_ClusterCnv_p1  chanCnv;
    typedef ITPConverterFor<Trk::PrepRawData> CONV;

//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " Reading " << persCont->m_collections.size() << "Collections" << endmsg;
    for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

        // Create trans collection - is NOT owner of SCT_Cluster (SG::VIEW_ELEMENTS)
        // IDet collection don't have the Ownership policy c'tor
        const Tracker::TrackerPRD_Collection_p1& pcoll = persCont->m_collections[icoll];        
        //Identifier collID(Identifier(pcoll.m_id));
        IdentifierHash collIDHash(IdentifierHash(pcoll.m_hashId));
        coll = new Tracker::FaserSCT_ClusterCollection(collIDHash);
        coll->setIdentifier(Identifier(pcoll.m_id));
        unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
        coll->resize(nchans);
        const TrackerDD::SiDetectorElement * de = (m_useDetectorElement ? elements->getDetectorElement(collIDHash) : nullptr);
        // Fill with channels
        for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
            const TPObjRef pchan = persCont->m_PRD[ichan + pcoll.m_begin];
            Tracker::FaserSCT_Cluster* chan = dynamic_cast<Tracker::FaserSCT_Cluster*>(createTransFromPStore((CONV**)0, pchan, log ) );
            if (chan) {
                chan->m_detEl = de;
                (*coll)[ichan] = chan;
            }
        }
        
        // register the rdo collection in IDC with hash - faster addCollection
        StatusCode sc = transCont->addCollection(coll, collIDHash);
        if (sc.isFailure()) {
            throw std::runtime_error("Failed to add collection to ID Container");
        }
//         if (log.level() <= MSG::DEBUG) {
//             log << MSG::DEBUG << "AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = " << (int) collIDHash << " / " << 
// collID.get_compact() << ", added to Identifiable container." << endmsg;
//         }
    }

//     if (log.level() <= MSG::DEBUG) log << MSG::DEBUG  << " ***  Reading SCT_ClusterContainer" << endmsg;
}



//================================================================
Tracker::FaserSCT_ClusterContainer* Tracker::FaserSCT_ClusterContainerCnv_p1::createTransient(const Tracker::TrackerPRD_Container_p1* persObj, MsgStream& log) 
{
    if(!m_isInitialized) {
     if (this->initialize(log) != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not initialize SCT_ClusterContainerCnv_p1 " << endmsg;
     } 
    }
    std::unique_ptr<Tracker::FaserSCT_ClusterContainer> trans(std::make_unique<Tracker::FaserSCT_ClusterContainer>(m_sctId->wafer_hash_max()));
    persToTrans(persObj, trans.get(), log);
    return(trans.release());
}

StatusCode Tracker::FaserSCT_ClusterContainerCnv_p1::initialize(MsgStream &log) {
   // Do not initialize again:
   m_isInitialized=true;
   
   // Get Storegate, ID helpers, and so on
   ISvcLocator* svcLocator = Gaudi::svcLocator();
   // get StoreGate service
   StatusCode sc = svcLocator->service("StoreGateSvc", m_storeGate);
   if (sc.isFailure()) {
      log << MSG::FATAL << "StoreGate service not found !" << endmsg;
      return StatusCode::FAILURE;
   }

   // get DetectorStore service
   StoreGateSvc *detStore;
   sc = svcLocator->service("DetectorStore", detStore);
   if (sc.isFailure()) {
      log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
      return StatusCode::FAILURE;
   } 
   //   else {
   //      if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found DetectorStore." << endmsg;
   //   }

   // Get the SCT helper from the detector store
   sc = detStore->retrieve(m_sctId, "FaserSCT_ID");
   if (sc.isFailure()) {
      log << MSG::FATAL << "Could not get SCT_ID helper !" << endmsg;
      return StatusCode::FAILURE;
   } 
   //   else {
   //      if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Found the SCT_ID helper." << endmsg;
   //   }

   CHECK(m_SCTDetEleCollKey.initialize(m_useDetectorElement));

   //   if (log.level() <= MSG::DEBUG) log << MSG::DEBUG << "Converter initialized." << endmsg;
   return StatusCode::SUCCESS;
}

// Method for test/SCT_ClusterContainerCnv_p1_test.cxx
void Tracker::FaserSCT_ClusterContainerCnv_p1::setUseDetectorElement(const bool useDetectorElement) {
   m_useDetectorElement = useDetectorElement;
}
