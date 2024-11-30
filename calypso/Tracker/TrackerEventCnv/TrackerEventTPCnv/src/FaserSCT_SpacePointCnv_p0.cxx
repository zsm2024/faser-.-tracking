/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
   */

#include "TrackerEventTPCnv/FaserSCT_SpacePointCnv_p0.h"
// Athena
#include "AthLinks/ElementLink.h"

#include "Identifier/Identifier.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"

StatusCode FaserSCT_SpacePointCnv_p0::initialize(MsgStream& log ) {

//  ISvcLocator* svcLocator = Gaudi::svcLocator();

  // Get the messaging service, print where you are
  log << MSG::DEBUG << "FaserSCT_SpacePointCnv::initialize()" << endmsg;
  if(m_sctClusContName.initialize()!=StatusCode::SUCCESS)
    log << MSG::WARNING<< "FaserSCT_SpacePointCnv failed to initialize the sct cluster container" << endmsg;

  return StatusCode::SUCCESS;
}

void
FaserSCT_SpacePointCnv_p0::persToTrans(const FaserSCT_SpacePoint_p0* persObj, Tracker::FaserSCT_SpacePoint* transObj, MsgStream& log) {

  if(this->initialize(log)!=StatusCode::SUCCESS){
    log << MSG::WARNING << "failed to initialize FaserSCT_SpacePointCnv_p0" << endmsg;
    return;
  }

  ElementLink<Tracker::FaserSCT_ClusterContainer> link1;
  ElementLink<Tracker::FaserSCT_ClusterContainer> link2;
  m_elCnv.persToTrans(&persObj->m_link1,&link1,log);
  m_elCnv.persToTrans(&persObj->m_link2,&link2,log);
  Amg::Vector3D pos(persObj->m_pos_x,persObj->m_pos_y,persObj->m_pos_z);
  Amg::Vector2D localpos(persObj->m_localpos_x,persObj->m_localpos_y);
  Amg::MatrixX localcov(2,2);
  localcov<<
    persObj->m_localcov00, persObj->m_localcov01,
    persObj->m_localcov01, persObj->m_localcov11;
  Amg::MatrixX cov(3,3);
  cov<<
    persObj->m_cov00, persObj->m_cov01, persObj->m_cov02,
    persObj->m_cov01, persObj->m_cov11, persObj->m_cov12,
    persObj->m_cov02, persObj->m_cov12, persObj->m_cov22;
  const Tracker::FaserSCT_Cluster* clus1=nullptr;
  const Tracker::FaserSCT_Cluster* clus2=nullptr;

  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> h_sctClusCont(m_sctClusContName);
  if (!h_sctClusCont.isValid()) {
    log<<MSG::ERROR<<"SCT Cluster container not found at "<<m_sctClusContName<<endmsg;
    return ;
  } else {
    log<<MSG::DEBUG<<"SCT Cluster Container found" <<endmsg;
  }
  const Tracker::FaserSCT_ClusterCollection *ptr = h_sctClusCont->indexFindPtr(IdentifierHash(persObj->m_idHash0));
  // if we find PRD, then recreate link
  if (ptr!=nullptr) {
    //loop though collection to find matching PRD.
    Tracker::FaserSCT_ClusterCollection::const_iterator collIt= ptr->begin();
    Tracker::FaserSCT_ClusterCollection::const_iterator collItEnd = ptr->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; ++collIt) {
      if ( (*collIt)->identify().get_compact()==persObj->m_id0 ) clus1= &(**collIt);
    }
  }

  const Tracker::FaserSCT_ClusterCollection *ptr1 = h_sctClusCont->indexFindPtr(IdentifierHash(persObj->m_idHash1));
  // if we find PRD, then recreate link
  if (ptr1!=nullptr) {
    //loop though collection to find matching PRD.
    Tracker::FaserSCT_ClusterCollection::const_iterator collIt= ptr1->begin();
    Tracker::FaserSCT_ClusterCollection::const_iterator collItEnd = ptr1->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; ++collIt) {
      if ( (*collIt)->identify().get_compact()==persObj->m_id1 ) clus2= &(**collIt);
    }
  }
  /*
     m_cnvtool->recreateSpacePoint(transObj,persObj->m_id0,persObj->m_idHash0,persObj->m_id1,persObj->m_idHash1);
     */
  //  if(clus2==nullptr)std::cout<<"didnot found the second cluster"<<std::endl;
  if(clus1==nullptr||clus2==nullptr){
    log<<MSG::DEBUG<<"could not found the clusters"<<endmsg;
    return;
  }
  //  const TrackerDD::SiDetectorElement * de1 = elements->getDetectorElement(  persObj->m_idHash0);
  //  const TrackerDD::SiDetectorElement * de2 = elements->getDetectorElement(  persObj->m_idHash1);
  //  Tracker::FaserSCT_Cluster* clus1=const_cast<Tracker::FaserSCT_Cluster*>(link1.cachedElement());
  //  Tracker::FaserSCT_Cluster* clus2=const_cast<Tracker::FaserSCT_Cluster*>(link2.cachedElement());
  //  clus1->setDetectorElement(de1);
  //  clus2->setDetectorElement(de2);
  //  *transObj=Tracker::FaserSCT_SpacePoint(std::make_pair(persObj->m_idHash0,persObj->m_idHash1),&pos,&cov,&localpos,&localcov,link1,link2);
  const std::pair<const Tracker::FaserSCT_Cluster*, const Tracker::FaserSCT_Cluster*> cluslist(clus1,clus2);
  //  transObj=new Tracker::FaserSCT_SpacePoint(std::make_pair(IdentifierHash(persObj->m_idHash0),IdentifierHash(persObj->m_idHash1)),&pos,&cluslist);
  //  //*transObj=Tracker::FaserSCT_SpacePoint(std::make_pair(IdentifierHash(persObj->m_idHash0),IdentifierHash(persObj->m_idHash1)),&pos,&cluslist);
  //  transObj->m_elelink1=link1;
  //  transObj->m_elelink1=link2;
  transObj->setElemIdList(std::make_pair(IdentifierHash(persObj->m_idHash0),IdentifierHash(persObj->m_idHash1)));
  transObj->setClusList(cluslist);
  transObj->setGlobalParameters(pos);
  transObj->setGlobalCovariance(cov);
  transObj->setLocalCovariance(localcov);
  transObj->setLocalParameters(localpos);

}

void
FaserSCT_SpacePointCnv_p0::transToPers(const Tracker::FaserSCT_SpacePoint* transObj, FaserSCT_SpacePoint_p0* persObj, MsgStream& /*log*/) {

  auto idHashs = transObj->elementIdList();
  persObj->m_idHash0 = idHashs.first;
  persObj->m_idHash1 = idHashs.second;

  auto globalPosition = transObj->globalPosition();
  persObj->m_pos_x = globalPosition.x();
  persObj->m_pos_y = globalPosition.y();
  persObj->m_pos_z = globalPosition.z();

  auto cov = transObj->globCovariance();
  persObj->m_cov00 = cov(0,0);
  persObj->m_cov01 = cov(0,1);
  persObj->m_cov02 = cov(0,2);
  persObj->m_cov11 = cov(1,1);
  persObj->m_cov12 = cov(1,2);
  persObj->m_cov22 = cov(2,2);

  persObj->m_id0= transObj->clusterList().first->identify().get_compact();
  persObj->m_id1= transObj->clusterList().second->identify().get_compact();
  auto localparam=transObj->localParameters();
  persObj->m_localpos_x = localparam.x();
  persObj->m_localpos_y = localparam.y();
  auto localcov = transObj->localCovariance();
  persObj->m_localcov00 = cov(0,0);
  persObj->m_localcov01 = cov(0,1);
  persObj->m_localcov11 = cov(1,2);

  persObj->m_link1.m_contName = transObj->getElementLink1()->dataID();
  persObj->m_link1.m_elementIndex = transObj->getElementLink1()->index();
  persObj->m_link2.m_contName = transObj->getElementLink2()->dataID();
  persObj->m_link2.m_elementIndex = transObj->getElementLink2()->index();
}
