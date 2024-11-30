/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
   */

#include "TrackerEventTPCnv/FaserSCT_SpacePointContainerCnv_p0.h"

#include "TrackerEventTPCnv/FaserSCT_SpacePoint_p0.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointCollection_p0.h"
#include "TrackerEventTPCnv/FaserSCT_SpacePointCnv_p0.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/StatusCode.h"

void FaserSCT_SpacePointContainerCnv_p0::persToTrans(const FaserSCT_SpacePointContainer_p0* persObj, FaserSCT_SpacePointContainer* transObj, MsgStream& log) {

  FaserSCT_SpacePointCollection* coll = 0;
  FaserSCT_SpacePointCnv_p0 chanCnv;
  unsigned int collBegin(0);

  log<<MSG::VERBOSE<<" create Spacepoint container with "<<persObj->m_spacepoint_collections.size()<<" collections"<<endmsg;

  for (unsigned int icoll = 0; icoll < persObj->m_spacepoint_collections.size(); ++icoll) {
    const FaserSCT_SpacePointCollection_p0& pcoll = persObj->m_spacepoint_collections[icoll];
    IdentifierHash collIDHash(IdentifierHash(pcoll.m_idHash));
    coll = new FaserSCT_SpacePointCollection(collIDHash);
    coll->setIdentifier(Identifier(pcoll.m_id));
    log<<MSG::VERBOSE<<" create Spacepoint collection with identifier "<<pcoll.m_id<<" , and hash "<<pcoll.m_idHash<<" , and size ="<<pcoll.m_end - pcoll.m_begin<<endmsg;
    unsigned int nchans           = pcoll.m_end - pcoll.m_begin;
    coll->resize(nchans);
    for (unsigned int ichan = 0; ichan < nchans; ++ ichan) {
      const FaserSCT_SpacePoint_p0* pchan=&(persObj->m_spacepoints[ichan+collBegin]);
      Tracker::FaserSCT_SpacePoint* transSP=new Tracker::FaserSCT_SpacePoint();
      chanCnv.persToTrans(pchan, transSP, log );
      if(transSP==nullptr)
	log << MSG::INFO<<"did not found space"<<endmsg;
      (*coll)[ichan]=transSP;
    }
    collBegin +=nchans;
    StatusCode sc = transObj->addCollection(coll, collIDHash);
    if (sc.isFailure()) 
      throw std::runtime_error("Failed to add collection to ID Container");

  }

}

void FaserSCT_SpacePointContainerCnv_p0::transToPers(const FaserSCT_SpacePointContainer* transObj, FaserSCT_SpacePointContainer_p0* persObj, MsgStream& log) {
  log << MSG::DEBUG<< "FaserSCT_SpacePointContainerCnv_p0::transToPers()" << endmsg;

  FaserSCT_SpacePointCnv_p0 spCnv;
  typedef FaserSCT_SpacePointContainer TRANS;

  TRANS::const_iterator spColl;
  TRANS::const_iterator spCollEnd = transObj->end();
  unsigned int spCollId;
  unsigned int spId = 0;
  unsigned int nextSpId = 0;
  //unsigned int lastIdHash = 0;

  // resize data vectors
  persObj->m_spacepoint_collections.resize(transObj->numberOfCollections());
  int n_spacepoints = 0; 
  for (spColl = transObj->begin(); spColl != spCollEnd; spColl++)  {
    n_spacepoints += (*spColl)->size();
  }
  persObj->m_spacepoints.resize(n_spacepoints);

  for (spColl = transObj->begin(), spCollId = 0; spColl != spCollEnd; ++spCollId, ++spColl)  {
    // create persitent representation of FaserSCT_SpacePointCollections
    const FaserSCT_SpacePointCollection& collection = **spColl;
    FaserSCT_SpacePointCollection_p0& pcollection = persObj->m_spacepoint_collections[spCollId];
    pcollection.m_size = collection.size();
    pcollection.m_idHash = (unsigned int)collection.identifyHash();
    pcollection.m_id = collection.identify().get_compact();
    log<<MSG::VERBOSE<<"Trans to Pers:  create Spacepoint collection with identifier "<<collection.identify().get_compact()<<" , and hash "<<collection.identifyHash()<<" , and size ="<<collection.size()<<endmsg;
    //lastIdHash = collection.identifyHash();

    spId = nextSpId;
    nextSpId += collection.size();

    pcollection.m_begin=spId;
    pcollection.m_end=nextSpId;

    // create persitent representation of FaserSCT_SpacePoints
    for (std::size_t i = 0; i < collection.size(); ++i) {
      FaserSCT_SpacePoint_p0* persSP = &(persObj->m_spacepoints[i + spId]);
      const Tracker::FaserSCT_SpacePoint* transSP = dynamic_cast<const Tracker::FaserSCT_SpacePoint*>(collection[i]);
      spCnv.transToPers(transSP, persSP, log);
    }
  }
}

//================================================================
FaserSCT_SpacePointContainer* FaserSCT_SpacePointContainerCnv_p0::createTransient(const FaserSCT_SpacePointContainer_p0* persObj, MsgStream& log) {
  log << MSG::DEBUG << "FaserSCT_SpacePointContainerCnv_p3::createTransient called " << endmsg;
  std::unique_ptr<FaserSCT_SpacePointContainer> trans(std::make_unique<FaserSCT_SpacePointContainer>(persObj->m_spacepoint_collections.size()));
  persToTrans(persObj, trans.get(), log);
  return(trans.release());
}
