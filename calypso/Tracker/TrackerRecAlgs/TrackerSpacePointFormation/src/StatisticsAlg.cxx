/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/***************************************************************************
  -------------------
  ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "StatisticsAlg.h"

// For processing clusters
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerReadoutGeometry/SiDetectorDesign.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

// Space point Classes,
 #include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
// #include "TrackerSpacePoint/TrackerSpacePointCLASS_DEF.h"
#include "TrackerIdentifier/FaserSCT_ID.h"


// general Atlas classes
#include "FaserDetDescr/FaserDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaMonitoringKernel/Monitored.h"

namespace Tracker
{
//------------------------------------------------------------------------
StatisticsAlg::StatisticsAlg(const std::string& name,
    ISvcLocator* pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator)
  , m_hist_x(0)
  , m_hist_y(0)
  , m_hist_z(0)
  , m_hist_r(0)
  , m_hist_eta(0)
  , m_hist_phi(0)
  , m_hist_station(0)
  , m_hist_strip(0)
  , m_hist_layer(0)
  , m_hist_x_y_plane0(0)
  , m_hist_x_y_plane1(0)
  , m_hist_x_y_plane2(0)
  , m_hist_x_y_plane3(0)
  , m_hist_x_y_plane4(0)
  , m_hist_x_y_plane5(0)
  , m_hist_x_y_plane6(0)
  , m_hist_x_y_plane7(0)
  , m_hist_x_y_plane8(0)
  , m_thistSvc("THistSvc", name)
{ 
}

//-----------------------------------------------------------------------
StatusCode StatisticsAlg::initialize()
{
  ATH_MSG_DEBUG( "StatisticsAlg::initialize()" );

  CHECK(m_thistSvc.retrieve());
  if(m_Sct_clcontainerKey.key().empty()){
    ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Sct_clcontainerKey.initialize() );
  // Check that clusters, space points and ids have names
  if ( m_Sct_spcontainerKey.key().empty()){
    ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
    return StatusCode::FAILURE;
  }
  ATH_CHECK( m_Sct_spcontainerKey.initialize() );

  // create containers (requires the Identifier Helpers)
  ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));

  // Initialize the key of input SiElementPropertiesTable and SiDetectorElementCollection for SCT
  ATH_CHECK(m_SCTDetEleCollKey.initialize());


  m_hist_x=new TH1D("sp_x","sp_x",100,-200,200);
  m_hist_y=new TH1D("sp_y","sp_y",100,-200,200);
  m_hist_z=new TH1D("sp_z","sp_z",100,0,300);
  m_hist_r=new TH1D("sp_r","sp_r",100,0,300);
  m_hist_eta=new TH1D("sp_eta","sp_eta",100,0,5);
  m_hist_phi=new TH1D("sp_phi","sp_phi",100,-3.2,3.2);
  m_hist_strip=new TH1D("sp_strip","sp_strip",100,0,2000);
  m_hist_layer=new TH1D("sp_layer","sp_layer",100,-10,10);
  m_hist_station=new TH1D("sp_station","sp_station",100,-10,10);
  m_hist_x_y_plane0=new TH2D("sp_x_y_plane0","sp_x_y_plane0",100,-200,200,100,-200,200);
  m_hist_x_y_plane1=new TH2D("sp_x_y_plane1","sp_x_y_plane1",100,-200,200,100,-200,200);
  m_hist_x_y_plane2=new TH2D("sp_x_y_plane2","sp_x_y_plane2",100,-200,200,100,-200,200);
  m_hist_x_y_plane3=new TH2D("sp_x_y_plane3","sp_x_y_plane3",100,-200,200,100,-200,200);
  m_hist_x_y_plane4=new TH2D("sp_x_y_plane4","sp_x_y_plane4",100,-200,200,100,-200,200);
  m_hist_x_y_plane5=new TH2D("sp_x_y_plane5","sp_x_y_plane5",100,-200,200,100,-200,200);
  m_hist_x_y_plane6=new TH2D("sp_x_y_plane6","sp_x_y_plane6",100,-200,200,100,-200,200);
  m_hist_x_y_plane7=new TH2D("sp_x_y_plane7","sp_x_y_plane7",100,-200,200,100,-200,200);
  m_hist_x_y_plane8=new TH2D("sp_x_y_plane8","sp_x_y_plane8",100,-200,200,100,-200,200);
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x",m_hist_x));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_y",m_hist_y));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_z",m_hist_z));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_r",m_hist_r));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_eta",m_hist_eta));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_phi",m_hist_phi));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_strip",m_hist_strip));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_layer",m_hist_layer));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_station",m_hist_station));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane0",m_hist_x_y_plane0));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane1",m_hist_x_y_plane1));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane2",m_hist_x_y_plane2));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane3",m_hist_x_y_plane3));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane4",m_hist_x_y_plane4));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane5",m_hist_x_y_plane5));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane6",m_hist_x_y_plane6));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane7",m_hist_x_y_plane7));
  CHECK(m_thistSvc->regHist("/StatisticsAlg/sp/sp_x_y_plane8",m_hist_x_y_plane8));
  ATH_MSG_INFO( "StatisticsAlg::initialized" );
  return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

StatusCode StatisticsAlg::execute (const EventContext& ctx) const
{


  ++m_numberOfEvents;
  const TrackerDD::SiDetectorElementCollection* elements = nullptr;


  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
  elements = sctDetEle.retrieve();
  if (elements==nullptr) {
    ATH_MSG_FATAL("Pointer of SiDetectorElementCollection (" << m_SCTDetEleCollKey.fullKey() << ") could not be retrieved");
    return StatusCode::SUCCESS;
  }

  // register the IdentifiableContainer into StoreGate

  //retrieve SCT cluster container
  SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
  if (!sct_clcontainer.isValid()){
  msg(MSG:: FATAL) << "Could not find the data object "<< sct_clcontainer.name() << " !" << endmsg;
  return StatusCode::RECOVERABLE;
  }
  std::cout<<"read sct"<<std::endl;
  Tracker::FaserSCT_ClusterContainer::const_iterator clusit = sct_clcontainer->begin();
  Tracker::FaserSCT_ClusterContainer::const_iterator clusend = sct_clcontainer->end();

  for (; clusit != clusend; ++clusit){
    const Tracker::FaserSCT_ClusterCollection *colNext=&(**clusit);
    Identifier elementID = colNext->identify();
    std::cout<<"SCT hash "<<colNext->identify()<<" "<<colNext->identifyHash()<<" "<<colNext->size()<<std::endl;
    FaserSCT_ClusterCollection::const_iterator clusters {colNext->begin()};
    FaserSCT_ClusterCollection::const_iterator clustersEnd {colNext->end()};
    for (; clusters != clustersEnd; ++clusters)
      {

    const TrackerDD::SiDetectorElement* elem = (*clusters)->detectorElement();
    if (elem == nullptr)std::cout<<"not found detectorelement "<<std::endl;
    else std::cout<<"element id "<<elem->identify();
      }
  }

  std::cout<<"finish read sct"<<std::endl;

  // retrieve SCT cluster container
  SG::ReadHandle<FaserSCT_SpacePointContainer> sct_spcontainer( m_Sct_spcontainerKey, ctx );
  if (!sct_spcontainer.isValid()){
    msg(MSG:: FATAL) << "Could not find the data object "<< sct_spcontainer.name() << " !" << endmsg;
    return StatusCode::RECOVERABLE;
  }


  ATH_MSG_DEBUG( "SCT spacepoint container found: " << sct_spcontainer->size() << " collections" );
  FaserSCT_SpacePointContainer::const_iterator it = sct_spcontainer->begin();
  FaserSCT_SpacePointContainer::const_iterator itend = sct_spcontainer->end();

  for (; it != itend; ++it){
    const FaserSCT_SpacePointCollection *colNext=&(**it);
    // int nReceivedSPSCT = colNext->size();

    // Create SpacePointCollection
    // IdentifierHash idHash = colNext->identifyHash();
    Identifier elementID = colNext->identify();
    int istation = m_idHelper->station(elementID);
    int ilayer = m_idHelper->layer(elementID);
    m_hist_strip->Fill(m_idHelper->strip(elementID));
    m_hist_station->Fill(istation);
    m_hist_layer->Fill(ilayer);
    size_t size = colNext->size();
    if (size == 0){
      ATH_MSG_VERBOSE( "StatisticsAlg algorithm found no space points" );
    } else {
      //In a MT environment the cache maybe filled by another thread in which case this will delete the duplicate
      FaserSCT_SpacePointCollection::const_iterator sp_begin= colNext->begin();
      FaserSCT_SpacePointCollection::const_iterator sp_end= colNext->end();
      for (; sp_begin != sp_end; ++sp_begin){
	const FaserSCT_SpacePoint* sp=&(**sp_begin);
	m_hist_r->Fill(sp->r());
	m_hist_eta->Fill(sp->eta());
	m_hist_phi->Fill(sp->phi());
	Amg::Vector3D gloPos=sp->globalPosition();
	m_hist_x->Fill(gloPos.x());
	ATH_MSG_DEBUG( "SCT spacepoint positions in container : " << gloPos.x()<<" "<<gloPos.y()<<" "<<gloPos.z() );
	m_hist_y->Fill(gloPos.y());
	m_hist_z->Fill(gloPos.z());
	if ( ((istation-1)*3+ilayer) == 0 ) m_hist_x_y_plane0->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 1 ) m_hist_x_y_plane1->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 2 ) m_hist_x_y_plane2->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 3 ) m_hist_x_y_plane3->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 4 ) m_hist_x_y_plane4->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 5 ) m_hist_x_y_plane5->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 6 ) m_hist_x_y_plane6->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 7 ) m_hist_x_y_plane7->Fill(gloPos.x(),gloPos.y());
	if ( ((istation-1)*3+ilayer) == 8 ) m_hist_x_y_plane8->Fill(gloPos.x(),gloPos.y());
      }
      ATH_MSG_VERBOSE( size << " SpacePoints successfully added to Container !" );
    }
  }


  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode StatisticsAlg::finalize()
{
  ATH_MSG_INFO( "StatisticsAlg::finalize()" );
  ATH_MSG_INFO( m_numberOfEvents << " events processed" );
  ATH_MSG_INFO( m_numberOfSCT << " sct collections processed" );
  ATH_MSG_INFO( m_numberOfSP<< " sct SP collections processed" );
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

}
