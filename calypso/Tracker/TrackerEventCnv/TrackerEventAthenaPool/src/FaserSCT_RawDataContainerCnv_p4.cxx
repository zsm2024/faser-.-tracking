/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_RawDataContainerCnv_p4.h"


#include "MsgUtil.h"
#include "FaserSCT1_RawDataCnv_p2.h"
#include "FaserSCT3_RawDataCnv_p4.h"

#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerEventAthenaPool/FaserSCT3_RawData_p4.h"
#include "TrackerEventAthenaPool/TrackerRawDataCollection_p1.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerRawData/FaserSCT_RDO_Collection.h"
#include "TrackerRawData/FaserSCT3_RawData.h"

#include <memory>

//#define SCT_DEBUG

void FaserSCT_RawDataContainerCnv_p4::transToPers(const FaserSCT_RDO_Container* transCont, FaserSCT_RawDataContainer_p4* persCont, MsgStream& log)
{

  /// The transient model has a container holding collections and the
  /// collections hold channels.
  ///
  /// The persistent model flattens this so that the persistent
  /// container has two vectors:
  ///   1) all collections, and
  ///   2) all RDO
  ///
  /// The persistent collections, then only maintain indexes into the
  /// container's vector of all channels. 
  ///
  /// So here we loop over all collection and add their channels
  /// to the container's vector, saving the indexes in the
  /// collection. 
  
  typedef FaserSCT_RDO_Container TRANS;
  
  FaserSCT1_RawDataCnv_p2  chan1Cnv;
  FaserSCT3_RawDataCnv_p4  chan3Cnv(m_sctId);
  TRANS::const_iterator it_Coll     = transCont->begin();
  TRANS::const_iterator it_CollEnd  = transCont->end();
  unsigned int collIndex;
  unsigned int chanBegin = 0;
  unsigned int chanEnd = 0;
  int numColl = transCont->numberOfCollections();
  persCont->m_collections.resize(numColl);
#ifdef SCT_DEBUG
  MSG_DEBUG(log,"FaserSCT_RawDataContainerCnv_p4  Preparing " << persCont->m_collections.size() << "Collections");
#endif

  /** we're going to store all the strip errors from each RDO 
   * in vectors in the collection */

  for (collIndex = 0; it_Coll != it_CollEnd; ++collIndex, it_Coll++)  {
    /** Add in new collection */
    const FaserSCT_RDO_Collection& collection = (**it_Coll);
    chanBegin  = chanEnd;
    chanEnd   += collection.size();
    TrackerRawDataCollection_p1& pcollection = persCont->m_collections[collIndex];
    pcollection.m_id    = collection.identify().get_identifier32().get_compact();
    pcollection.m_hashId = static_cast<unsigned int>(collection.identifyHash());
    pcollection.m_begin = chanBegin;
    pcollection.m_end   = chanEnd;
#ifdef SCT_DEBUG
    MSG_DEBUG(log,"Writing collection number " << collIndex << " with hash" << pcollection.m_hashId);
#endif
    /** Add in channels */
    if (m_type == 1) {
#ifdef SCT_DEBUG
      MSG_DEBUG(log,"FaserSCT_RawDataContainerCnv_p4 m_type is 1");
#endif
      persCont->m_rawdata.resize(chanEnd);
      for (unsigned int i = 0; i < collection.size(); ++i) {
        TrackerRawData_p2* pchan = &(persCont->m_rawdata[i + chanBegin]);
        const FaserSCT1_RawData* chan = dynamic_cast<const FaserSCT1_RawData*>(collection[i]);
        chan1Cnv.transToPers(chan, pchan, log);
      }            
    } else if (m_type == 3) {
#ifdef SCT_DEBUG
      MSG_DEBUG(log,"FaserSCT_RawDataContainerCnv_p4 m_type is 3");
#endif
      persCont->m_sct3data.resize(chanEnd);
      for (unsigned int i = 0; i < collection.size(); ++i) {
        FaserSCT3_RawData_p4* pchan = &(persCont->m_sct3data[i + chanBegin]);
        const FaserSCT3_RawData* chan = dynamic_cast<const FaserSCT3_RawData*>(collection[i]);
        if (nullptr == chan) throw std::runtime_error("FaserSCT_RawDataContainerCnv_p4::transToPers: *** UNABLE TO DYNAMIC CAST TO FaserSCT3_RawData");
        chan3Cnv.transToPers(chan, pchan, log);
      }
    }
  }
#ifdef SCT_DEBUG
  MSG_DEBUG(log," ***  Writing FaserSCT_RDO_Container (SCT1/3_RawData concrete type)");
#endif
}

void  FaserSCT_RawDataContainerCnv_p4::persToTrans(const FaserSCT_RawDataContainer_p4* persCont, FaserSCT_RDO_Container* transCont, MsgStream& log)
{

  /// The transient model has a container holding collections and the
  /// collections hold channels.
  ///
  /// The persistent model flattens this so that the persistent
  /// container has two vectors:
  ///   1) all collections, and
  ///   2) all channels
  ///
  /// The persistent collections, then only maintain indexes into the
  /// container's vector of all channels. 
  ///
  /// So here we loop over all collection and extract their channels
  /// from the vector.


  FaserSCT1_RawDataCnv_p2  chan1Cnv;
  FaserSCT3_RawDataCnv_p4  chan3Cnv(m_sctId);
  /** check for the type of the contained objects: */
    
  if (persCont->m_rawdata.size() !=0 && persCont->m_sct3data.size() != 0) {
    log << MSG::FATAL << "The collection has mixed SCT1 and SCT3 elements, this is not allowed " << endmsg;
  }
  if (persCont->m_rawdata.size() != 0 ) m_type = 1;
  if (persCont->m_sct3data.size() != 0 ) m_type = 3;
#ifdef SCT_DEBUG
  MSG_DEBUG(log," Reading " << persCont->m_collections.size() << " Collections");
#endif
  for (unsigned int icoll = 0; icoll < persCont->m_collections.size(); ++icoll) {

    /** Create trans collection - in NOT owner of SCT_RDO_RawData (SG::VIEW_ELEMENTS)
     * IDet collection don't have the Ownership policy c'tor */
    const TrackerRawDataCollection_p1& pcoll = persCont->m_collections[icoll];
    Identifier collID(pcoll.m_id);
    chan3Cnv.setWaferId(collID);
    IdentifierHash collIDHash(pcoll.m_hashId);
    std::unique_ptr<FaserSCT_RDO_Collection> coll = std::make_unique<FaserSCT_RDO_Collection>(IdentifierHash(collIDHash));
    coll->setIdentifier(collID);
    unsigned int nchans = pcoll.m_end - pcoll.m_begin;

    coll->resize(nchans);
#ifdef SCT_DEBUG
    MSG_DEBUG(log,"Reading collection with id = " << pcoll.m_id << " id hash= " << pcoll.m_hashId
              << " and num el= " << nchans << " m_type is "<<m_type);
#endif
    // Fill with channels
    for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
      if (m_type == 1) {
        const TrackerRawData_p2* pchan = &(persCont->m_rawdata[ichan + pcoll.m_begin]);
        std::unique_ptr<FaserSCT1_RawData> chan = std::make_unique<FaserSCT1_RawData>();
        chan1Cnv.persToTrans(pchan, chan.get(), log);
        (*coll)[ichan] = chan.release();
      } else if (m_type == 3) {
        const FaserSCT3_RawData_p4* pchan = &(persCont->m_sct3data[ichan + pcoll.m_begin]);
        std::unique_ptr<FaserSCT3_RawData> chan = std::make_unique<FaserSCT3_RawData>();
        chan3Cnv.persToTrans(pchan, chan.get(), log);
        (*coll)[ichan] = chan.release();
      }
    }

    // register the rdo collection in IDC with hash - faster addCollection
    StatusCode sc = transCont->addCollection(coll.release(), collIDHash);
    if (sc.isFailure()) {
      throw std::runtime_error("Failed to add collection to ID Container");
    }
    MSG_VERBOSE(log,"AthenaPoolTPCnvIDCont::persToTrans, collection, hash_id/coll id = "
                << collIDHash.value() << " / " << collID.get_compact() << ", added to Identifiable container.");

  }
#ifdef SCT_DEBUG
  MSG_DEBUG(log," ***  Reading FaserSCT_RDO_Container (SCT1/3_RawData concrete type)");
#endif
}

//================================================================
FaserSCT_RDO_Container* FaserSCT_RawDataContainerCnv_p4::createTransient(const FaserSCT_RawDataContainer_p4* persObj, MsgStream& log) {
#ifdef SCT_DEBUG
  MSG_DEBUG(log,"creating transient FaserSCT_RDO_Container");
#endif
  std::unique_ptr<FaserSCT_RDO_Container> trans(std::make_unique<FaserSCT_RDO_Container>(m_sctId->wafer_hash_max()));
  persToTrans(persObj, trans.get(), log);
  return trans.release();
}
