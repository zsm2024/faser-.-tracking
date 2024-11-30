/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/***************************************************************************
  -------------------
  ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "TrackerSeedFinder/TrackerSeedFinder.h"

// For processing clusters
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerReadoutGeometry/SiDetectorDesign.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

// Space point Classes,
#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

// general Atlas classes
#include "FaserDetDescr/FaserDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaMonitoringKernel/Monitored.h"

//!!!!!!!!!!!!!!!!!!!!!!!!
//#include "Acts/EventData/TrackParameters.hpp"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "Acts/Utilities/detail/periodic.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
using namespace Acts::UnitLiterals;
//!!!!!!!!!!!!!!!!!!!!!!!!
namespace Tracker
{

  //------------------------------------------------------------------------
  TrackerSeedFinder::TrackerSeedFinder(const std::string& name,
				       ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
    , m_hist_n(0)
    , m_hist_x(0)
    , m_hist_y(0)
    , m_hist_z(0)
    , m_hist_r(0)
    , m_hist_phi(0)
    , m_hist_eta(0)
    , m_hist_layer(0)
    , m_hist_strip(0)
    , m_hist_station(0)
    , m_hist_x_z(0)
    , m_hist_y_z(0)
    , m_hist_sp_plane(0)
    , m_hist_sp_layer(0)
    , m_hist_sp_station(0)
    , m_hist_nseed(0)
    , m_hist_nsp1(0)
    , m_hist_nsp2(0)
    , m_thistSvc("THistSvc", name)
  {}

  //-----------------------------------------------------------------------
  StatusCode TrackerSeedFinder::initialize() {

    ATH_MSG_DEBUG( "TrackerSeedFinder::initialize()" );

    CHECK(m_thistSvc.retrieve());
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
    if (m_seed_spcontainerKey.key().empty()){
      ATH_MSG_FATAL( "No name set for output seeds space points");
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_seed_spcontainerKey.initialize() );

    ATH_CHECK( m_trackerSeedContainerKey.initialize() );
    if (m_trackerSeedContainerKey.key().empty()){
      ATH_MSG_FATAL( "No name set for output track seeds");
      return StatusCode::FAILURE;
    }

    m_hist_n=new TH1D("sp_n","sp_n",20,0,20);
    m_hist_x=new TH1D("sp_x","sp_x",100,-200,200);
    m_hist_y=new TH1D("sp_y","sp_y",100,-200,200);
    m_hist_z=new TH1D("sp_z","sp_z",3500,0,3500);
    m_hist_r=new TH1D("sp_r","sp_r",100,0,200);
    m_hist_eta=new TH1D("sp_eta","sp_eta",100,0,5);
    m_hist_phi=new TH1D("sp_phi","sp_phi",100,-3.2,3.2);
    m_hist_strip=new TH1D("sp_strip","sp_strip",1000,0,1000);
    m_hist_layer=new TH1D("sp_layer","sp_layer",100,-10,10);
    m_hist_station=new TH1D("sp_station","sp_station",100,-10,10);
    m_hist_sp_station=new TH1D("sp_all_station","sp_station",100,-10,10);
    m_hist_nseed=new TH1D("sp_nseed","n track seeds",1000,0,1000);
    m_hist_nsp1=new TH1D("sp_nsp1","n triplets in first station",100,0,100);
    m_hist_nsp2=new TH1D("sp_nsp2","n triplets in second station",100,0,100);
    m_hist_sp_plane=new TH1D("sp_all_plane","sp_station",10,0,10);
    m_hist_sp_layer=new TH1D("sp_all_layer","sp_station",100,-10,10);
    m_hist_x_z=new TH2D("sp_x_z","sp_x_z",100,-200,200,3500,0,3500);
    m_hist_y_z=new TH2D("sp_y_z","sp_y_z",100,-200,200,3500,0,3500);
    m_hist_dx=new TH1D("sp_dx","sp_dx",100,-200,200);
    m_hist_dy=new TH1D("sp_dy","sp_dy",100,-200,200);
    m_hist_dx_slope=new TH1D("sp_dx_slope","sp_dx_slope",100,-200,200);
    m_hist_dy_slope=new TH1D("sp_dy_slope","sp_dy_slope",100,-200,200);
    m_hist_dx_intercept=new TH1D("sp_dx_intercept","sp_dx_intercept",100,-200,200);
    m_hist_dx_intercept_after=new TH1D("sp_dx_intercept_after","sp_dx_intercept_after",100,-200,200);
    m_hist_line=new TH1D("sp_line","sp_line",100,-200,200);
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_n",m_hist_n));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_x",m_hist_x));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_y",m_hist_y));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_z",m_hist_z));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_r",m_hist_r));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_eta",m_hist_eta));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_phi",m_hist_phi));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_strip",m_hist_strip));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_layer",m_hist_layer));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_station",m_hist_station));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_all_station",m_hist_sp_station));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_nseed",m_hist_nseed));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_nsp1",m_hist_nsp1));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_nsp2",m_hist_nsp2));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_all_layer",m_hist_sp_layer));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_all_plane",m_hist_sp_plane));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_x_z",m_hist_x_z));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_y_z",m_hist_y_z));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_dx",m_hist_dx));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_dy",m_hist_dy));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_dx_slope",m_hist_dx_slope));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_dy_slope",m_hist_dy_slope));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_dx_intercept",m_hist_dx_intercept));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_dx_intercept_after",m_hist_dx_intercept_after));
    CHECK(m_thistSvc->regHist("/TrackerSeedFinder/sp/sp_line",m_hist_line));

    ATH_MSG_INFO( "TrackerSeedFinder::initialized" );
    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------

  StatusCode TrackerSeedFinder::make_triplets(vector<vector<const FaserSCT_SpacePoint*> >& vsp, vector<seed>& vt, string st) const {

    int count=0;   
    double xs=0, ys=0, zs=0, z2s=0, xzs=0, yzs=0;

    for (unsigned int i=0; i<vsp[0].size(); i++) {
      for (unsigned int j=0; j<vsp[1].size(); j++) {
    	for (unsigned int k=0; k<vsp[2].size(); k++) {

    	  ATH_MSG_VERBOSE( " station " << st << " / list of space points for seeds " 
			   << vsp[0].at(i)->clusterList().first->identify() << " " 
			   << vsp[1].at(j)->clusterList().first->identify() << " " 
			   << vsp[2].at(k)->clusterList().first->identify());

	  count++;

	  vector<double> x; vector<double> y; vector<double> z;
	  x.push_back(vsp[0].at(i)->globalPosition().x()); x.push_back(vsp[1].at(j)->globalPosition().x()); x.push_back(vsp[2].at(k)->globalPosition().x());
	  y.push_back(vsp[0].at(i)->globalPosition().y()); y.push_back(vsp[1].at(j)->globalPosition().y()); y.push_back(vsp[2].at(k)->globalPosition().y());
	  z.push_back(vsp[0].at(i)->globalPosition().z()); z.push_back(vsp[1].at(j)->globalPosition().z()); z.push_back(vsp[2].at(k)->globalPosition().z());

	  xs=x.at(0)+x.at(1)+x.at(2); ys=y.at(0)+y.at(1)+y.at(2); zs=z.at(0)+z.at(1)+z.at(2);
	  z2s=pow(z.at(0),2)+pow(z.at(1),2)+pow(z.at(2),2);
	  xzs=x.at(0)*z.at(0)+x.at(1)*z.at(1)+x.at(2)*z.at(2);
	  yzs=y.at(0)*z.at(0)+y.at(1)*z.at(1)+y.at(2)*z.at(2);

	  seed mt;
	  mt.axz=(3*xzs-xs*zs)/(3*z2s-zs*zs);
	  mt.bxz=(z2s*xs-xzs*zs)/(3*z2s-zs*zs);
	  mt.ayz=(3*yzs-ys*zs)/(3*z2s-zs*zs);
	  mt.byz=(z2s*ys-yzs*zs)/(3*z2s-zs*zs);
	  mt.add_sp(vsp[0].at(i)); mt.add_sp(vsp[1].at(j)); mt.add_sp(vsp[2].at(k));
	  mt.station=st;
	  mt.num=count;

	  for(int pos=0; pos<3; pos++) {
	    mt.chi2_xz+=pow(x.at(pos)-(mt.axz*z.at(pos)+mt.bxz),2);
	    mt.chi2_yz+=pow(x.at(pos)-(mt.ayz*z.at(pos)+mt.byz),2);
	  }

	  ATH_MSG_VERBOSE( "  x1 " << x.at(0) << "; x2 " << x.at(1) << "; x3 " << x.at(2));
	  ATH_MSG_VERBOSE( "  y1 " << y.at(0) << "; y2 " << y.at(1) << "; y3 " << y.at(2));
	  ATH_MSG_VERBOSE( "  z1 " << z.at(0) << "; z2 " << z.at(1) << "; z3 " << z.at(2));
	  ATH_MSG_VERBOSE( "  linear fit on plane xz / slope " << mt.axz << "; intercept " << mt.bxz);
	  ATH_MSG_VERBOSE( "  linear fit on plane yz / slope " << mt.ayz << "; intercept " << mt.byz);
	  ATH_MSG_VERBOSE( "  chi2 / on plane xz " << mt.chi2_xz << "; on plane yz " << mt.chi2_yz);

	  vt.push_back(mt);

    	}
      }
    }
 
    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------

  StatusCode TrackerSeedFinder::execute (const EventContext& ctx) const {

    ++m_numberOfEvents;
    N_1_0=0, N_1_1=0, N_1_2=0, N_2_0=0, N_2_1=0, N_2_2=0, N_3_0=0, N_3_1=0, N_3_2=0;
    int nseed=0,nsp1=0,nsp2=0;

    // retrieve tracker spacepoint container
    SG::ReadHandle<FaserSCT_SpacePointContainer> sct_spcontainer( m_Sct_spcontainerKey, ctx );
    if (!sct_spcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_spcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    ATH_MSG_DEBUG( "Tracker spacepoint container found: " << sct_spcontainer->size() << " collections" );
    FaserSCT_SpacePointContainer::const_iterator it = sct_spcontainer->begin();
    FaserSCT_SpacePointContainer::const_iterator itend = sct_spcontainer->end();


    SG::WriteHandle<TrackerSeedCollection> seedContainer(m_trackerSeedContainerKey, ctx);
    ATH_CHECK(seedContainer.record( std::make_unique<TrackerSeedCollection>() ) );
    ATH_MSG_DEBUG("Created track seed container " << m_trackerSeedContainerKey.key());
    int i=0;

    vector<vector<const FaserSCT_SpacePoint*> > vsp1(3);
    vector<vector<const FaserSCT_SpacePoint*> > vsp2(3);
    vector<vector<const FaserSCT_SpacePoint*> > vsp3(3);

    vector<pair<int, const FaserSCT_SpacePoint*>> msp;

    vector<vector<pair<int, const FaserSCT_SpacePoint*>>> mseeds;
    vector<int> pos;

    ATH_MSG_DEBUG( "Start iteration of spacepoint collections"  );
    for (; it != itend; ++it){

      ++m_numberOfSPCollection;
      const FaserSCT_SpacePointCollection *colNext=&(**it);
      int nReceivedSPSCT = colNext->size();

      // Create FaserSCT_SpacePointCollection
      IdentifierHash idHash = colNext->identifyHash();
      Identifier elementID = colNext->identify();

      ATH_MSG_DEBUG( "Spacepoint collection in iteraction: size =  " <<nReceivedSPSCT <<", IDHash = "<<idHash<<", ID = "<<elementID );
      if (nReceivedSPSCT== 0){
	ATH_MSG_VERBOSE( "Algorithm found no space points" );
	++m_numberOfEmptySPCollection;
      } else {
	FaserSCT_SpacePointCollection::const_iterator sp_begin= colNext->begin();
	FaserSCT_SpacePointCollection::const_iterator sp_end= colNext->end();
	
	ATH_MSG_DEBUG( "Iterate the spacepoint in collection "  );

	for (; sp_begin != sp_end; ++sp_begin) {

	  ++i;
	  ++m_numberOfSP;
	  const FaserSCT_SpacePoint* sp=&(**sp_begin);

	  const auto identifier = sp->clusterList().first->identify();
	  int station = m_idHelper->station(identifier);
	  int plane = m_idHelper->layer(identifier);

	  ATH_MSG_VERBOSE( " layer " << (station-1)*3+plane ); 
	  msp.push_back(make_pair((station-1)*3+plane,sp));
	  if ((station-1)*3+plane < 3) {
	    vsp1.at(plane).push_back(sp);
	  } else if ((station-1)*3+plane < 6) {
	    vsp2.at(plane).push_back(sp);
	  } else if ((station-1)*3+plane < 9) {
	    vsp3.at(plane).push_back(sp);
	  }

	  m_hist_sp_plane->Fill((station-1)*3+plane);
	  m_hist_sp_station->Fill(station);
	  m_hist_sp_layer->Fill(plane);

	  if (station==1 && plane==0) {N_1_0++;} if (station==1 && plane==1) {N_1_1++;} if (station==1 && plane==2) {N_1_2++;}
	  if (station==2 && plane==0) {N_2_0++;} if (station==2 && plane==1) {N_2_1++;} if (station==2 && plane==2) {N_2_2++;}
	  if (station==3 && plane==0) {N_3_0++;} if (station==3 && plane==1) {N_3_1++;} if (station==3 && plane==2) {N_3_2++;}

	  pos.push_back((station-1)*3+plane);

	  ATH_MSG_VERBOSE( " spacepoint n " << m_numberOfSP );
	  ATH_MSG_VERBOSE( "  station " << station << " plane " << plane << " pos " << pos.at(i-1) << " index " << (station-1)*3+plane);
	  ATH_MSG_VERBOSE( "  i " << i << "  x " << sp->globalPosition().x() << " y " << sp->globalPosition().y() << " z " << sp->globalPosition().z());

	  m_hist_r->Fill(sp->r());
	  m_hist_eta->Fill(sp->eta());
	  m_hist_phi->Fill(sp->phi());

	  m_hist_x->Fill(sp->globalPosition().x());
	  m_hist_y->Fill(sp->globalPosition().y());
	  m_hist_z->Fill(sp->globalPosition().z());

	  m_hist_x_z->Fill(sp->globalPosition().x(),sp->globalPosition().z());
	  m_hist_y_z->Fill(sp->globalPosition().y(),sp->globalPosition().z());

	}
      }
    }

    ATH_MSG_VERBOSE( " number of space points; event " << m_numberOfEvents);
    ATH_MSG_VERBOSE( "  1st station " << N_1_0+N_1_1+N_1_2 << " (" << vsp1.at(0).size() << ";" << vsp1.at(1).size() << ";" << vsp1.at(2).size() << ")");
    ATH_MSG_VERBOSE( "  2nd station " << N_2_0+N_2_1+N_2_2 << " (" << vsp2.at(0).size() << ";" << vsp2.at(1).size() << ";" << vsp2.at(2).size() << ")");
    ATH_MSG_VERBOSE( "  3rd station " << N_3_0+N_3_1+N_3_2 << " (" << vsp3.at(0).size() << ";" << vsp3.at(1).size() << ";" << vsp3.at(2).size() << ")");
    if((N_1_0+N_1_1+N_1_2)<3||(N_2_0+N_2_1+N_2_2)<3)
      return StatusCode::SUCCESS;

    vector<seed> vseed;
    ATH_CHECK(make_triplets(vsp1, vseed, "1"));
    ATH_CHECK(make_triplets(vsp2, vseed, "2"));

    vector<seed> vfseed;
    for (vector<seed>::iterator it1 = vseed.begin(); it1 != vseed.end(); ++it1) {
      if((*it1).station=="1")
      nsp1++;
      if((*it1).station=="2")
      nsp2++;
      for (vector<seed>::iterator it2 = vseed.begin(); it2 != vseed.end(); ++it2) {

	if ((*it1).station == "1" && (*it2).station == "2") {

	  ATH_MSG_VERBOSE( "Considering seeds " << (*it1).num << " and " << (*it2).num);

	  seed ms;
	  ms.station="12";
	  ms.add_vsp((*it1).vsp);
	  ms.add_vsp((*it2).vsp);
	  vfseed.push_back(ms);
	}
      }
    }
    

    for (vector<seed>::iterator it = vfseed.begin(); it != vfseed.end(); ++it) {

      if ((*it).station != "12") continue;

      nseed++;

      vector<const FaserSCT_SpacePoint*> mseed = (*it).vsp;

      Tracker::TrackerSeed* trackerSeed = new Tracker::TrackerSeed();
      trackerSeed->set_id(TrackerSeed::TRIPLET_SP_FIRSTSTATION);
      trackerSeed->add(mseed);

      seedContainer->push_back(trackerSeed);
    }

      m_hist_nsp1->Fill(nsp1);
      m_hist_nsp2->Fill(nsp2);
      m_hist_nseed->Fill(nseed);
    ATH_MSG_DEBUG("Saved " << nseed << " seeds in container");

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
    StatusCode TrackerSeedFinder::finalize()
  {
    ATH_MSG_INFO( "Finalizing" );
    ATH_MSG_INFO( m_numberOfEvents << " events processed" );
    ATH_MSG_INFO( m_numberOfSPCollection << " spacepoint collections processed" );
    ATH_MSG_INFO( m_numberOfEmptySPCollection<< " spacepoint collections empty" );
    ATH_MSG_INFO( m_numberOfSP<< " sct SP collections processed" );

    ATH_MSG_INFO( "Finalizing TrackerSeedFinder" );

    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

}
