/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/***************************************************************************
  -------------------
  ATLAS Collaboration
 ***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "TruthSeededTrackFinder.h"

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
  TruthSeededTrackFinder::TruthSeededTrackFinder(const std::string& name,
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
      , m_hist_x_y_plane0(0)
      , m_hist_x_y_plane1(0)
      , m_hist_x_y_plane2(0)
      , m_hist_x_y_plane3(0)
      , m_hist_x_y_plane4(0)
      , m_hist_x_y_plane5(0)
      , m_hist_x_y_plane6(0)
      , m_hist_x_y_plane7(0)
      , m_hist_x_y_plane8(0)
      , m_hist_x_z(0)
      , m_hist_y_z(0)
      , m_hist_sp_sensor(0)
      , m_hist_sp_module (0)
      , m_hist_sp_row (0)
      , m_hist_sp_plane (0)
      , m_hist_sp_layer (0)
      , m_hist_sp_station (0)
	//!!!!!!!!!!!!!!!!!!!!
      , m_hist_InitReso_px (0)
      , m_hist_InitReso_py (0)
      , m_hist_InitReso_pz (0)
	//!!!!!!!!!!!!!!!!!!!!

      , m_thistSvc("THistSvc", name)
      { 
      }

  //-----------------------------------------------------------------------
  StatusCode TruthSeededTrackFinder::initialize()
  {
    //
    ATH_MSG_DEBUG( "TruthTrackSeeds::initialize()" );

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

    if ( m_seed_spcontainerKey.key().empty()){
      ATH_MSG_FATAL( "No name set for output seeds space points");
      return StatusCode::FAILURE;
    }

    ATH_CHECK( m_seed_spcontainerKey.initialize() );

    ATH_CHECK( m_mcEventKey.initialize() );
    ATH_CHECK( m_faserSiHitKey.initialize() );
    ATH_CHECK( m_faserRdoKey.initialize());
    ATH_CHECK( m_sctMap.initialize());
    ATH_MSG_DEBUG( "Using GenEvent collection with key " << m_mcEventKey.key());
    ATH_MSG_DEBUG( "Using Faser SiHit collection with key " << m_faserSiHitKey.key());
    ATH_MSG_DEBUG( "Using FaserSCT RDO Container with key " << m_faserRdoKey.key());
    ATH_MSG_DEBUG( "Using SCT_SDO_Map with key "<< m_sctMap.key());

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
    m_hist_sp_row=new TH1D("sp_all_row","sp_station",100,-10,10);
    m_hist_sp_module=new TH1D("sp_all_module","sp_station",100,-10,10);
    m_hist_sp_sensor=new TH1D("sp_all_sensor","sp_station",100,-10,10);
    m_hist_sp_plane=new TH1D("sp_all_plane","sp_station",100,-10,10);
    m_hist_sp_layer=new TH1D("sp_all_layer","sp_station",100,-10,10);
    m_hist_x_z=new TH2D("sp_x_z","sp_x_z",100,-200,200,3500,0,3500);
    m_hist_y_z=new TH2D("sp_y_z","sp_y_z",100,-200,200,3500,0,3500);
    m_hist_x_y_plane0=new TH2D("sp_x_y_plane0","sp_x_y_plane0",100,-200,200,100,-200,200);
    m_hist_x_y_plane1=new TH2D("sp_x_y_plane1","sp_x_y_plane1",100,-200,200,100,-200,200);
    m_hist_x_y_plane2=new TH2D("sp_x_y_plane2","sp_x_y_plane2",100,-200,200,100,-200,200);
    m_hist_x_y_plane3=new TH2D("sp_x_y_plane3","sp_x_y_plane3",100,-200,200,100,-200,200);
    m_hist_x_y_plane4=new TH2D("sp_x_y_plane4","sp_x_y_plane4",100,-200,200,100,-200,200);
    m_hist_x_y_plane5=new TH2D("sp_x_y_plane5","sp_x_y_plane5",100,-200,200,100,-200,200);
    m_hist_x_y_plane6=new TH2D("sp_x_y_plane6","sp_x_y_plane6",100,-200,200,100,-200,200);
    m_hist_x_y_plane7=new TH2D("sp_x_y_plane7","sp_x_y_plane7",100,-200,200,100,-200,200);
    m_hist_x_y_plane8=new TH2D("sp_x_y_plane8","sp_x_y_plane8",100,-200,200,100,-200,200);
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_n",m_hist_n));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x",m_hist_x));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_y",m_hist_y));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_z",m_hist_z));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_r",m_hist_r));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_eta",m_hist_eta));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_phi",m_hist_phi));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_strip",m_hist_strip));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_layer",m_hist_layer));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_station",m_hist_station));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_all_station",m_hist_sp_station));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_all_layer",m_hist_sp_layer));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_all_module",m_hist_sp_module));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_all_plane",m_hist_sp_plane));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_all_row",m_hist_sp_row));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_all_sensor",m_hist_sp_sensor));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_z",m_hist_x_z));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_y_z",m_hist_y_z));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane0",m_hist_x_y_plane0));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane1",m_hist_x_y_plane1));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane2",m_hist_x_y_plane2));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane3",m_hist_x_y_plane3));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane4",m_hist_x_y_plane4));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane5",m_hist_x_y_plane5));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane6",m_hist_x_y_plane6));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane7",m_hist_x_y_plane7));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/sp_x_y_plane8",m_hist_x_y_plane8));
	//!!!!!!!!!!!!!!!!!!!!
    m_hist_InitReso_px=new TH1D("InitReso_px","InitReso_px",200,-10,10);
    m_hist_InitReso_py=new TH1D("InitReso_py","InitReso_py",200,-10,10);
    m_hist_InitReso_pz=new TH1D("InitReso_pz","InitReso_pz",200,-10,10);
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/InitReso_px",m_hist_InitReso_px));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/InitReso_py",m_hist_InitReso_py));
    CHECK(m_thistSvc->regHist("/TruthTrackSeeds/sp/InitReso_pz",m_hist_InitReso_pz));
	//!!!!!!!!!!!!!!!!!!!!
    ATH_MSG_DEBUG( "TruthTrackSeeds::initialized" );
    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------

  StatusCode TruthSeededTrackFinder::execute (const EventContext& ctx) const
  {


    ++m_numberOfEvents;

    // Handles created from handle keys behave like pointers to the corresponding container
    SG::ReadHandle<McEventCollection> h_mcEvents(m_mcEventKey, ctx);
    ATH_MSG_DEBUG("Read McEventContainer with " << h_mcEvents->size() << " events");
    if (h_mcEvents->size() == 0) return StatusCode::FAILURE;

    SG::ReadHandle<FaserSiHitCollection> h_siHits(m_faserSiHitKey, ctx);
    ATH_MSG_DEBUG("Read FaserSiHitCollection with " << h_siHits->size() << " hits");

    SG::ReadHandle<FaserSCT_RDO_Container> h_sctRDO(m_faserRdoKey, ctx);


    SG::WriteHandle<SpacePointForSeedCollection> seedContainer_SCT(m_seed_spcontainerKey, ctx );

    ATH_CHECK( seedContainer_SCT.record( std::make_unique<SpacePointForSeedCollection>() ) );
    ATH_MSG_DEBUG("Created SpacePointContainer " << m_seed_spcontainerKey.key() << " N= " << m_idHelper->wafer_hash_max());


    const TrackerDD::SiDetectorElementCollection* elements = nullptr;
    SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
    elements = sctDetEle.retrieve();
    if (elements==nullptr) {
      ATH_MSG_FATAL("Pointer of SiDetectorElementCollection (" << m_SCTDetEleCollKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }


    // retrieve SCT cluster container
    SG::ReadHandle<FaserSCT_SpacePointContainer> sct_spcontainer( m_Sct_spcontainerKey, ctx );
    if (!sct_spcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_spcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

	//!!!!!!!!!!!!!!!!!!!!
    static const TrackerDD::SCT_DetectorManager     *s_sct;
    if(detStore()->retrieve(s_sct,"SCT").isFailure()) s_sct = 0;
	int N_1_0=0, N_1_1=0, N_1_2=0, N_2_0=0, N_2_1=0, N_2_2=0;
	int match_N_1_0=0, match_N_1_1=0, match_N_1_2=0, match_N_2_0=0, match_N_2_1=0, match_N_2_2=0;
        HepMC::FourVector truthmom;
	//!!!!!!!!!!!!!!!!!!!!

    ATH_MSG_DEBUG( "SCT spacepoint container found: " << sct_spcontainer->size() << " collections" );
    FaserSCT_SpacePointContainer::const_iterator it = sct_spcontainer->begin();
    FaserSCT_SpacePointContainer::const_iterator itend = sct_spcontainer->end();

    int nTruthSP=0;
    ATH_MSG_DEBUG( "Start iteration of spacepoint collections"  );
    for (; it != itend; ++it){
      ++m_numberOfSPCollection;
      const FaserSCT_SpacePointCollection *colNext=&(**it);
      int nReceivedSPSCT = colNext->size();

      // Create SpacePointCollection
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
	for (; sp_begin != sp_end; ++sp_begin){
	  ++m_numberOfSP;
	  const FaserSCT_SpacePoint* sp=&(**sp_begin);
	  bool match=false;
	  //get truth hit
	  const auto identifier = sp->clusterList().first->identify();
	  int station = m_idHelper->station(identifier);
	  int plane = m_idHelper->layer(identifier);
	  int row = m_idHelper->phi_module(identifier);
	  int module = m_idHelper->eta_module(identifier);
	  int sensor = m_idHelper->side(identifier);
	  m_hist_sp_station->Fill(station);
	  m_hist_sp_plane->Fill(plane);
	  m_hist_sp_layer->Fill((station-1)*3+plane);
	  m_hist_sp_row->Fill(row);
	  m_hist_sp_module->Fill(module);
	  m_hist_sp_sensor->Fill(sensor);
	//!!!!!!!!!!!!!!!!!!!!
	if (station==1 && plane==0) N_1_0++;
	if (station==1 && plane==1) N_1_1++;
	if (station==1 && plane==2) N_1_2++;
	if (station==2 && plane==0) N_2_0++;
	if (station==2 && plane==1) N_2_1++;
	if (station==2 && plane==2) N_2_2++;
	//!!!!!!!!!!!!!!!!!!!!

	  ATH_MSG_DEBUG( "Spacepoint :"<<identifier  );
	  SG::ReadHandle<TrackerSimDataCollection> h_collectionMap(m_sctMap, ctx);
	  ATH_MSG_DEBUG("map size "<<h_collectionMap->size());
	//!!!!!!!!!!!!!!!!!!!!
//		  std::cout<<"!!!!!!!!!  cluster: "<<sp->clusterList().first<<std::endl;
		  ATH_MSG_DEBUG("!!!!!!!!!  cluster: "<<sp->clusterList().first->rdoList() );
	//!!!!!!!!!!!!!!!!!!!!
	  if( h_collectionMap->count(identifier) == 0)
	  {
	    ATH_MSG_DEBUG("no map found w/identifier "<<identifier);
	    ++m_numberOfNoMap;
	    continue;
	  }

	  //Collection map takes identifier and returns simulation data
	  const auto& simdata = h_collectionMap->find(identifier)->second;
	  const auto& deposits = simdata.getdeposits();

	  //loop through deposits to find one w/ highest energy & get barcode
	  float highestDep = 0;
	  int barcode = 0;
	  HepMcParticleLink primary{};
	  for( const auto& depositPair : deposits)
	  {
	      //!!!!!!!!!!!!!!!!!!!!!!!!
//	      depositPair.first->print(std::cout);
//	        std::cout<<"!!!!!!!!!!!  pdg id =  "<<depositPair.first->pdg_id()<<std::endl;
		if (depositPair.first->pdg_id() == -13) {
        // HepMC::FourVector pv = depositPair.first->production_vertex()->position();
        // HepMC::FourVector ev = depositPair.first->end_vertex()->position ();
	        truthmom = depositPair.first->momentum();
        // std::cout<<"!!!!!!!!!!!  production_vertex: ( "<<pv.x()<<",  "<<pv.y()<<",  "<<pv.z()<<" )  "<<std::endl;
        // std::cout<<"!!!!!!!!!!!  end_vertex: ( "<<ev.x()<<",  "<<ev.y()<<",  "<<ev.z()<<" )  "<<std::endl;
		}
	      //!!!!!!!!!!!!!!!!!!!!!!!!
	    if( depositPair.second > highestDep)
	    {
	      highestDep = depositPair.second;
	      primary = depositPair.first;
#ifdef HEPMC3
        barcode = primary->id();
	      ATH_MSG_DEBUG("pdg id "<<depositPair.first->pid());
#else
	      barcode = depositPair.first->barcode();
	      //!!!!depositPair.first->print(std::cout);
	      ATH_MSG_DEBUG("pdg id "<<depositPair.first->pdg_id());
#endif
	    }
	  }

	  ATH_MSG_DEBUG("final barcode of: "<<barcode);
	  if(barcode%1000000 != 10001) continue;

	  //!!!!!!!!!!!!!!!!!!!!!!!!
	  if (primary->end_vertex() != nullptr)
	   {
#ifdef HEPMC3
        for (auto daughter : primary->end_vertex()->particles_out())
        {
          if (daughter->id() % 1000000 == primary->id())
          {
     		     ATH_MSG_DEBUG("    daughter barcode: " << daughter->id() << " with energy " << daughter->momentum().e() << "  px = " << daughter->momentum().px() << "  py = " << daughter->momentum().py() << "  pz = " << daughter->momentum().pz() );
          }
        }

#else
	      for (auto daughter : primary->particles_out())
		{
		  // TODO: Check that daughter->production_vertex() and daughter->end_vertex() exist, and bracket the point you're interested in
	    if (daughter->barcode() % 1000000 == primary->barcode()) {
		     ATH_MSG_DEBUG("    daughter barcode: " << daughter->barcode() << " with energy " << daughter->momentum().e() << "  px = " << daughter->momentum().px() << "  py = " << daughter->momentum().py() << "  pz = " << daughter->momentum().pz() );
      // if (daughter->production_vertex() != nullptr && daughter->end_vertex() != nullptr) { 
  		// HepMC::FourVector pv = daughter->production_vertex()->position();
	  	// HepMC::FourVector ev = daughter->end_vertex()->position ();
      // std::cout<<"!!!!!!!!!!!  production_vertex: ( "<<pv.x()<<",  "<<pv.y()<<",  "<<pv.z()<<" )  "<<std::endl;
      // std::cout<<"!!!!!!!!!!!  end_vertex: ( "<<ev.x()<<",  "<<ev.y()<<",  "<<ev.z()<<" )  "<<std::endl;
  		//  }
		  }
		}
#endif

	   }
	   //!!!!!!!!!!!!!!!!!!!!!!!!

	  //Helper function to get hit location information from RDO identifier
	  ATH_MSG_DEBUG("trying to match hit to stat/plane/row/mod/sens: "<<station<<" "<<plane<<" "<<row<<" "<<module<<" "<<sensor);
	  for (const FaserSiHit& hit : *h_siHits)
	  {
	    ++m_numberOfHits;
	    ATH_MSG_DEBUG("hit w/vals "<<hit.getStation()<<" "<<hit.getPlane()<<" "<<hit.getRow()<<" "<<hit.getModule()<<" "<<hit.getSensor()<<" barcode: "<<hit.trackNumber());
	    //set of conditions to confirm looking at same particle in same place for SiHit as RDO
	    if(hit.getStation() == station 
		&& hit.getPlane() == plane
		&& hit.getRow() == row
		&& hit.getModule() == module
		&& hit.getSensor() == sensor
		&& hit.trackNumber() == barcode)
	    {
	      ++m_numberOfMatchSP;
	      ATH_MSG_DEBUG("matched particle and plotting w/ barcode "<<barcode);
	      match=true;
	    }
	      //!!!!!!!!!!!!!!!!!!!!!!!!
	const TrackerDD::SiDetectorElement *geoelement=NULL;
	Identifier  id = m_idHelper->wafer_id(hit.getStation(),
			    		 hit.getPlane(),
					 hit.getRow(),
					 hit.getModule(),
					 hit.getSensor() );

	geoelement = s_sct->getDetectorElement(id);
	if (geoelement) {
	double m_ran_pos=(double)(std::rand())/RAND_MAX;
	HepGeom::Point3D<double> m_lp_start,m_lp_end;
	HepGeom::Point3D<double> m_new_pos;
	//start and end position
	m_lp_start=hit.localStartPosition();
	m_lp_end=hit.localEndPosition();
	m_new_pos=m_lp_start+(m_lp_end-m_lp_start)*m_ran_pos;
	//muPos=Amg::EigenTransformToCLHEP(geoelement->transformHit()) * HepGeom::Point3D<double>(m_new_pos);
//	std::cout<<"!!!!!!!!!!!  hit start pos: "<<Amg::EigenTransformToCLHEP(geoelement->transformHit()) * HepGeom::Point3D<double>(m_lp_start)<<std::endl;
//	std::cout<<"!!!!!!!!!!!  hit end pos: "<<Amg::EigenTransformToCLHEP(geoelement->transformHit()) * HepGeom::Point3D<double>(m_lp_end)<<std::endl;
	}
	      //!!!!!!!!!!!!!!!!!!!!!!!!
	  }
	  //
	  if(!match)
	    continue;
	  ++m_numberOfFills;
	  ++nTruthSP;

	  SpacePointForSeed* spseed=new SpacePointForSeed(sp);
	  seedContainer_SCT->push_back(spseed);
	  m_hist_r->Fill(sp->r());
	  m_hist_eta->Fill(sp->eta());
	  m_hist_phi->Fill(sp->phi());
	  Amg::Vector3D gloPos=sp->globalPosition();
	  m_hist_x->Fill(gloPos.x());
	  m_hist_y->Fill(gloPos.y());
	  m_hist_z->Fill(gloPos.z());
	  m_hist_strip->Fill(m_idHelper->strip(elementID));
	  m_hist_station->Fill(m_idHelper->station(elementID));
	  m_hist_layer->Fill(m_idHelper->layer(elementID));
	  m_hist_x_z->Fill(gloPos.x(),gloPos.z());
	  m_hist_y_z->Fill(gloPos.y(),gloPos.z());
	  int ilayer=m_idHelper->layer(elementID);
	  int istation=m_idHelper->station(elementID);
	  if ( ((istation-1)*3+ilayer) == 0 ) m_hist_x_y_plane0->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 1 ) m_hist_x_y_plane1->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 2 ) m_hist_x_y_plane2->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 3 ) m_hist_x_y_plane3->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 4 ) m_hist_x_y_plane4->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 5 ) m_hist_x_y_plane5->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 6 ) m_hist_x_y_plane6->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 7 ) m_hist_x_y_plane7->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation-1)*3+ilayer) == 8 ) m_hist_x_y_plane8->Fill(gloPos.x(),gloPos.y());
	//!!!!!!!!!!!!!!!!!!!!
	if (istation==1 && ilayer==0) { match_N_1_0++; }
	if (istation==1 && ilayer==1) { match_N_1_1++; }
	if (istation==1 && ilayer==2) { match_N_1_2++; }
	if (istation==2 && ilayer==0) { match_N_2_0++; }
	if (istation==2 && ilayer==1) { match_N_2_1++; }
	if (istation==2 && ilayer==2) { match_N_2_2++; }
//	std::cout<<"!!!!!!!!!!! (station, layer) = ("<<istation<<", "<<ilayer<<")     positopn = ("<<gloPos.x()<<", "<<gloPos.y()<<", "<<gloPos.z()<<") "<<std::endl;
//       if (std::fabs(gloPos.x())>100) std::cout<<"!!!!!!!!!!! badXXX "<<std::endl;
	//!!!!!!!!!!!!!!!!!!!!
	}
	ATH_MSG_VERBOSE( nReceivedSPSCT << " SpacePoints successfully added to Container !" );
      }
    }

    m_hist_n->Fill(nTruthSP);

    //!!!!!!!!!!!!!!!!!!!!!!!!
// std::cout<<"!!!!!!!!!!!  N_1_0 = "<<N_1_0<<",  N_1_1 = "<<N_1_1<<",  N_1_2 = "<<N_1_2<<",  N_2_0 = "<<N_2_0<<",  N_2_1 = "<<N_2_1<<",  N_2_2 = "<<N_2_2<<std::endl;
// std::cout<<"!!!!!!!!!!!  match_N_1_0 = "<<match_N_1_0<<",  match_N_1_1 = "<<match_N_1_1<<",  match_N_1_2 = "<<match_N_1_2<<",  match_N_2_0 = "<<match_N_2_0<<",  match_N_2_1 = "<<match_N_2_1<<",  match_N_2_2 = "<<match_N_2_2<<std::endl;
  if ( (N_1_0==1 && match_N_1_0==1) && (N_1_1==1 && match_N_1_1==1) && (N_1_2==1 && match_N_1_2==1) && (N_2_0==1 && match_N_2_0==1) && (N_2_1==1 && match_N_2_1==1) && (N_2_2==1 && match_N_2_2==1)) {
  Acts::Vector3 pos1_0(0., 0., 0.);
  Acts::Vector3 pos1_1(0., 0., 0.);
  Acts::Vector3 pos1_2(0., 0., 0.);
  Acts::Vector3 pos2_0(0., 0., 0.);
  Acts::Vector3 pos2_1(0., 0., 0.);
  Acts::Vector3 pos2_2(0., 0., 0.);

  FaserSCT_SpacePointContainer::const_iterator it = sct_spcontainer->begin();
  FaserSCT_SpacePointContainer::const_iterator itend = sct_spcontainer->end();

  for (; it != itend; ++it){
    const FaserSCT_SpacePointCollection *colNext=&(**it);

    // Create SpacePointCollection
    IdentifierHash idHash = colNext->identifyHash();
    Identifier elementID = colNext->identify();
    int istation = m_idHelper->station(elementID);
    int ilayer = m_idHelper->layer(elementID);
    size_t size = colNext->size();
    if (size == 0){
      ATH_MSG_VERBOSE( "StatisticsAlg algorithm found no space points" );
    } else {
      //In a MT environment the cache maybe filled by another thread in which case this will delete the duplicate
      FaserSCT_SpacePointCollection::const_iterator sp_begin= colNext->begin();
      FaserSCT_SpacePointCollection::const_iterator sp_end= colNext->end();
      for (; sp_begin != sp_end; ++sp_begin){
	const FaserSCT_SpacePoint* sp=&(**sp_begin);
	Amg::Vector3D gloPos=sp->globalPosition();
	if ( ((istation-1)*3+ilayer) == 0 ) pos1_0 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
	if ( ((istation-1)*3+ilayer) == 1 ) pos1_1 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
	if ( ((istation-1)*3+ilayer) == 2 ) pos1_2 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
	if ( ((istation-1)*3+ilayer) == 3 ) pos2_0 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
	if ( ((istation-1)*3+ilayer) == 4 ) pos2_1 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
	if ( ((istation-1)*3+ilayer) == 5 ) pos2_2 = Acts::Vector3(gloPos.x(), gloPos.y(), gloPos.z());
      }
    }
  }
	
//  std::cout<<"!!!!!!!!!!!  pos1_0 = ("<<pos1_0.x()<<", "<<pos1_0.y()<<", "<<pos1_0.z()<<") "<<std::endl;
//  std::cout<<"!!!!!!!!!!!  pos1_1 = ("<<pos1_1.x()<<", "<<pos1_1.y()<<", "<<pos1_1.z()<<") "<<std::endl;
//  std::cout<<"!!!!!!!!!!!  pos1_2 = ("<<pos1_2.x()<<", "<<pos1_2.y()<<", "<<pos1_2.z()<<") "<<std::endl;
//  std::cout<<"!!!!!!!!!!!  pos2_0 = ("<<pos2_0.x()<<", "<<pos2_0.y()<<", "<<pos2_0.z()<<") "<<std::endl;
//  std::cout<<"!!!!!!!!!!!  pos2_1 = ("<<pos2_1.x()<<", "<<pos2_1.y()<<", "<<pos2_1.z()<<") "<<std::endl;
//  std::cout<<"!!!!!!!!!!!  pos2_2 = ("<<pos2_2.x()<<", "<<pos2_2.y()<<", "<<pos2_2.z()<<") "<<std::endl;

  double charge = 1;
  double B = 0.57;
  // const Acts::Vector3 pos = pos1_0;
  /*
  Acts::Vector3 d1 = pos1_2 - pos1_0;
  Acts::Vector3 d2 = pos2_2 - pos2_0;
  */

  // the least square of three space pionts
  // Note: when z2 = (z1 + z3)/2 , the slope is the same as (pos1_2 - pos1_0)
  // calculate the averages
  double Ax1 = (pos1_0.x() + pos1_1.x() +pos1_2.x()) / 3;
  double Ay1 = (pos1_0.y() + pos1_1.y() +pos1_2.y()) / 3;
  double Az1 = (pos1_0.z() + pos1_1.z() +pos1_2.z()) / 3;
  // calculate the sums
  double Sxz1 = pos1_0.x()*pos1_0.z() + pos1_1.x()*pos1_1.z() + pos1_2.x()*pos1_2.z();
  double Syz1 = pos1_0.y()*pos1_0.z() + pos1_1.y()*pos1_1.z() + pos1_2.y()*pos1_2.z();
  double Szz1 = pos1_0.z()*pos1_0.z() + pos1_1.z()*pos1_1.z() + pos1_2.z()*pos1_2.z();
  // calculate the slope
  double kxz1 = (Sxz1 - 3*Ax1*Az1) / (Szz1 - 3*Az1*Az1);
  double kyz1 = (Syz1 - 3*Ay1*Az1) / (Szz1 - 3*Az1*Az1);
  Acts::Vector3 d1( kxz1, kyz1, 1.0);
  // calculate the averages
  double Ax2 = (pos2_0.x() + pos2_1.x() +pos2_2.x()) / 3;
  double Ay2 = (pos2_0.y() + pos2_1.y() +pos2_2.y()) / 3;
  double Az2 = (pos2_0.z() + pos2_1.z() +pos2_2.z()) / 3;
  // calculate the sums
  double Sxz2 = pos2_0.x()*pos2_0.z() + pos2_1.x()*pos2_1.z() + pos2_2.x()*pos2_2.z();
  double Syz2 = pos2_0.y()*pos2_0.z() + pos2_1.y()*pos2_1.z() + pos2_2.y()*pos2_2.z();
  double Szz2 = pos2_0.z()*pos2_0.z() + pos2_1.z()*pos2_1.z() + pos2_2.z()*pos2_2.z();
  // calculate the slope
  double kxz2 = (Sxz2 - 3*Ax2*Az2) / (Szz2 - 3*Az2*Az2);
  double kyz2 = (Syz2 - 3*Ay2*Az2) / (Szz2 - 3*Az2*Az2);
  Acts::Vector3 d2( kxz2, kyz2, 1.0);
  //

  // the direction of momentum in the first station
  Acts::Vector3 direct1 = d1.normalized();
//  std::cout<<"!!!!!!!!!!!  direct1 = ("<<direct1.x()<<", "<<direct1.y()<<", "<<direct1.z()<<") "<<std::endl;
  // the direction of momentum in the second station
  Acts::Vector3 direct2 = d2.normalized();
//  std::cout<<"!!!!!!!!!!!  direct2 = ("<<direct2.x()<<", "<<direct2.y()<<", "<<direct2.z()<<") "<<std::endl;
  // the vector pointing from the center of circle to the particle at layer 2 in Y-Z plane
  double R1_z = charge * direct1.y() / std::sqrt(direct1.y()*direct1.y() + direct1.z()*direct1.z());
  // double R1_y = -charge * direct1.z() / std::sqrt(direct1.y()*direct1.y() + direct1.z()*direct1.z());
//  std::cout<<"!!!!!!!!!!!  (R1_y, R1_z) = ("<<R1_y<<", "<<R1_z<<") "<<std::endl;
  // the vector pointing from the center of circle to the particle at layer 3 in Y-Z plane
  double R2_z = charge * direct2.y() / std::sqrt(direct2.y()*direct2.y() + direct2.z()*direct2.z());
  // double R2_y = -charge * direct2.z() / std::sqrt(direct2.y()*direct2.y() + direct2.z()*direct2.z());
//  std::cout<<"!!!!!!!!!!!  (R2_y, R2_z) = ("<<R2_y<<", "<<R2_z<<") "<<std::endl;
  // the norm of radius
  double R = (pos2_0.z() - pos1_2.z()) / (R2_z - R1_z);
//  std::cout<<"!!!!!!!!!!!  R = "<<R<<std::endl;
  // the norm of momentum in Y-Z plane
  double p_yz = 0.3*B*R / 1000.0;  // R(mm), p(GeV), B(T)
  double p_z = p_yz * direct1.z() / std::sqrt(direct1.y()*direct1.y() + direct1.z()*direct1.z());
  double p_y = p_yz * direct1.y() / std::sqrt(direct1.y()*direct1.y() + direct1.z()*direct1.z());
  double p_x = direct1.x() * p_z / direct1.z();
  // total momentum at the layer 0
  const Acts::Vector3 mom(p_x, p_y, p_z);
  // double p = mom.norm();
//  std::cout<<"!!!!!!!!!!!  InitTrack momentum on layer 0: ( "<<mom.x()*1000<<",  "<<mom.y()*1000<<",  "<<mom.z()*1000<<",  "<<p*1000<<")  "<<std::endl;
//  std::cout<<"!!!!!!!!!!!  truth momentum: ( "<<truthmom.px()<<",  "<<truthmom.py()<<",  "<<truthmom.pz()<<",  "<<truthmom.m()<<" )  "<<std::endl;

  if ((p_x*1000 - truthmom.px()) / std::fabs(truthmom.px()) < -10) m_hist_InitReso_px->Fill( -9.5 );
    else if ((p_x*1000 - truthmom.px()) / std::fabs(truthmom.px()) > 10) m_hist_InitReso_px->Fill( 9.5 );
    else m_hist_InitReso_px->Fill( (p_x*1000 - truthmom.px()) / std::fabs(truthmom.px())  );

  if ((p_y*1000 - truthmom.py()) / std::fabs(truthmom.py()) < -10) m_hist_InitReso_py->Fill( -9.5 );
    else if ((p_y*1000 - truthmom.py()) / std::fabs(truthmom.py()) > 10) m_hist_InitReso_py->Fill( 9.5 );
    else m_hist_InitReso_py->Fill( (p_y*1000 - truthmom.py()) / std::fabs(truthmom.py())  );

  if ((p_z*1000 - truthmom.pz()) / std::fabs(truthmom.pz()) < -10) m_hist_InitReso_pz->Fill( -9.5 );
    else if ((p_z*1000 - truthmom.pz()) / std::fabs(truthmom.pz())>10) m_hist_InitReso_pz->Fill( 9.5 );
    else m_hist_InitReso_pz->Fill( (p_z*1000 - truthmom.pz()) / std::fabs(truthmom.pz())  );

 /*
  // build the track covariance matrix using the smearing sigmas
  double sigmaU = 20_um;
  double sigmaV = 20_um;
  double sigmaPhi = 1_degree;
  double sigmaTheta = 1_degree;
  double sigmaQOverP = 0.01*p / (p*p);
  double sigmaT0 = 1_ns;
  Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
  cov(Acts::eLOC_0, Acts::eLOC_0) = sigmaU * sigmaU;
  cov(Acts::eLOC_1, Acts::eLOC_1) = sigmaV * sigmaV;
  cov(Acts::ePHI, Acts::ePHI) = sigmaPhi * sigmaPhi;
  cov(Acts::eTHETA, Acts::eTHETA) = sigmaTheta * sigmaTheta;
  cov(Acts::eQOP, Acts::eQOP) = sigmaQOverP * sigmaQOverP;
  cov(Acts::eT, Acts::eT) = sigmaT0 * sigmaT0;

  double time =0;
  Acts::CurvilinearParameters InitTrackParam(std::make_optional(std::move(cov)), pos, mom, charge, time);
  */
  }
    //!!!!!!!!!!!!!!!!!!!!!!
    
    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  StatusCode TruthSeededTrackFinder::finalize()
  {
    ATH_MSG_DEBUG( "Finalizing" );
    ATH_MSG_DEBUG( m_numberOfEvents << " events processed" );
    ATH_MSG_DEBUG( m_numberOfSPCollection << " spacepoint collections processed" );
    ATH_MSG_DEBUG( m_numberOfEmptySPCollection<< " spacepoint collections empty" );
    ATH_MSG_DEBUG( m_numberOfSP<< " sct SP collections processed" );
    ATH_MSG_DEBUG( m_numberOfNoMap<< " not maped spacepoint" );
    ATH_MSG_DEBUG( m_numberOfHits<< " sim hits" );
    ATH_MSG_DEBUG( m_numberOfMatchSP<< " matched spacepoint" );
    ATH_MSG_DEBUG( m_numberOfFills<< " spacepoint saved" );
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

}
