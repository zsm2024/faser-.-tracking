/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerEventCnvTools/TrackerEventCnvTool.h"

#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"

#include <vector>
#include <cassert>
#include <iostream>

Tracker::TrackerEventCnvTool::TrackerEventCnvTool(const std::string& t,
                                                  const std::string& n,
                                                  const IInterface*  p )
  :
  base_class(t,n,p),
  m_setPrepRawDataLink(true),
  m_IDHelper(nullptr),
  m_SCTHelper(nullptr),
  m_idDictMgr(nullptr)
{
  declareProperty("RecreatePRDLinks", m_setPrepRawDataLink);
  
}

StatusCode Tracker::TrackerEventCnvTool::initialize() {

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;
  
  // check if SLHC geo is used (TRT not implemented) 
  // if not SLHC, get the TRT Det Descr manager
  sc = detStore()->retrieve(m_idDictMgr, "IdDict");
  if (sc.isFailure()) {
    std::cout << "Could not get IdDictManager !" << std::endl;
    return StatusCode::FAILURE;
  } 
  const IdDictDictionary* dict = m_idDictMgr->manager()->find_dictionary("Tracker");
  if (!dict) {
    std::cout << " Cannot access Tracker dictionary "<< std::endl;
    return StatusCode::FAILURE;
  }

  //retrieving the various ID helpers
  
  if (detStore()->retrieve(m_IDHelper, "FaserID").isFailure()) {
    ATH_MSG_FATAL( "Could not get FASER ID helper");
    return StatusCode::FAILURE;
  }

  ATH_CHECK( detStore()->retrieve(m_SCTHelper, "FaserSCT_ID") );

  ATH_CHECK( m_sctClusContName.initialize() );

  ATH_CHECK( m_SCTDetEleCollKey.initialize() );

  return sc;
     
}

void 
Tracker::TrackerEventCnvTool::checkRoT( const Trk::RIO_OnTrack& rioOnTrack ) const {
  TrackerConcreteType type=Unknown;
  if (0!=dynamic_cast<const FaserSCT_ClusterOnTrack*>(&rioOnTrack) )     type = SCT;
  if (type==Unknown) {
    ATH_MSG_ERROR("Type does not match known concrete type of Tracker! Dumping RoT:"<<rioOnTrack);
  } else {
    ATH_MSG_VERBOSE("Type = "<<type);
  }
    
  return;
}

std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*> 
Tracker::TrackerEventCnvTool::getLinks( Trk::RIO_OnTrack& rioOnTrack ) const
{
  using namespace Trk;
  const TrkDetElementBase* detEl = 0;
  const PrepRawData*       prd   = 0;
  const Identifier& id           = rioOnTrack.identify();
   
  if (m_IDHelper->is_sct(id)) {
    ATH_MSG_DEBUG("Set SCT detector element" );
    // use IdentifierHash for speed
    detEl = getSCTDetectorElement( rioOnTrack.idDE() ) ;
    if (m_setPrepRawDataLink) prd = sctClusterLink( id, rioOnTrack.idDE() );
  } else {
    ATH_MSG_WARNING("Unknown type of Tracker detector from identifier :"
                    << id<<", in string form:"
                    << m_IDHelper->show_to_string(id)
                    );
  }
  return std::pair<const Trk::TrkDetElementBase*, const Trk::PrepRawData*>(detEl,prd);
}

void Tracker::TrackerEventCnvTool::prepareRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
    
  Tracker::FaserSCT_ClusterOnTrack* sct = dynamic_cast<Tracker::FaserSCT_ClusterOnTrack*>(RoT);
  if (sct!=0) {
    prepareRIO_OnTrackElementLink<const Tracker::FaserSCT_ClusterContainer, Tracker::FaserSCT_ClusterOnTrack>(sct);
    return;
  }
  return;
}

void
Tracker::TrackerEventCnvTool::prepareRIO_OnTrackLink( const Trk::RIO_OnTrack *RoT,
                                                      ELKey_t& key,
                                                      ELIndex_t& index) const
{
  const Tracker::FaserSCT_ClusterOnTrack* sct = dynamic_cast<const Tracker::FaserSCT_ClusterOnTrack*>(RoT);
  if (sct!=0) {
    prepareRIO_OnTrackElementLink<const Tracker::FaserSCT_ClusterContainer, Tracker::FaserSCT_ClusterOnTrack>(sct, key, index);
    return;
  }
  return;
}

void Tracker::TrackerEventCnvTool::recreateRIO_OnTrack( Trk::RIO_OnTrack *RoT ) const {
  std::pair<const Trk::TrkDetElementBase *, const Trk::PrepRawData *> pair = getLinks( *RoT );
  Trk::ITrkEventCnvTool::setRoT_Values( pair, RoT );
  return;
}

const Trk::TrkDetElementBase* 
Tracker::TrackerEventCnvTool::getDetectorElement(const Identifier& id, const IdentifierHash& idHash) const {

  const Trk::TrkDetElementBase* detEl=0;

  if (m_IDHelper->is_sct(id)) {

    ATH_MSG_DEBUG("Set SCT detector element" );
    // use IdentifierHash for speed
    detEl = getSCTDetectorElement( idHash ) ;
  } else {
    ATH_MSG_WARNING("Unknown type of Tracker detector from identifier :"
                    << id<<", in string form:"
                    << m_IDHelper->show_to_string(id)
                    );
  }
  return detEl;
}

const Trk::TrkDetElementBase* 
Tracker::TrackerEventCnvTool::getDetectorElement(const Identifier& id) const {
 
  const Trk::TrkDetElementBase* detEl=0;
 
 if (m_IDHelper->is_sct(id)) {
    ATH_MSG_DEBUG("Set SCT detector element" );
    const Identifier wafer_id = m_SCTHelper->wafer_id(id);
    const IdentifierHash wafer_hash = m_SCTHelper->wafer_hash(wafer_id);
    detEl = getSCTDetectorElement( wafer_hash ) ;
  } else {
    ATH_MSG_WARNING("Unknown type of Tracker detector from identifier :"<< id<<", in string form:"
                    << m_IDHelper->show_to_string(id) );
  }
  return detEl;
}

const Trk::PrepRawData* 
Tracker::TrackerEventCnvTool::sctClusterLink( const Identifier& id,  const IdentifierHash& idHash ) const {
  using namespace Trk;
  // need to retrieve pointers to collections
  // retrieve Pixel cluster container
  
  // obviously this can be optimised! EJWM
  SG::ReadHandle<FaserSCT_ClusterContainer> h_sctClusCont(m_sctClusContName);
  if (!h_sctClusCont.isValid()) {
    ATH_MSG_ERROR("FaserSCT Cluster container not found at "<<m_sctClusContName);
    return 0;
  } else {
    ATH_MSG_DEBUG("FaserSCT Cluster Container found" );
  }
  const FaserSCT_ClusterCollection *ptr = h_sctClusCont->indexFindPtr(idHash);
  // if we find PRD, then recreate link
  if (ptr!=nullptr) {
    //loop though collection to find matching PRD.
    FaserSCT_ClusterCollection::const_iterator collIt    = ptr->begin();
    FaserSCT_ClusterCollection::const_iterator collItEnd = ptr->end();
    // there MUST be a faster way to do this!!
    for ( ; collIt!=collItEnd; collIt++) {
      if ( (*collIt)->identify()==id ) return *collIt;
    }
  }
  ATH_MSG_DEBUG("No matching PRD found" );
  return 0;
}

const TrackerDD::SiDetectorElement* Tracker::TrackerEventCnvTool::getSCTDetectorElement(const IdentifierHash& waferHash) const {
  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle{m_SCTDetEleCollKey};
  if (not sctDetEle.isValid()) return nullptr;
  return sctDetEle->getDetectorElement(waferHash);
}

