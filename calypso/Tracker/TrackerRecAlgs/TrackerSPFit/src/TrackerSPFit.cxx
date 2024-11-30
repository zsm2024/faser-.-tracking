/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

/***************************************************************************
  -------------------
 ***************************************************************************/

//<<<<<< INCLUDES >>>>>>


#include "TrackerSPFit.h"

// For processing clusters
#include "TrackerReadoutGeometry/SiLocalPosition.h"
#include "TrackerReadoutGeometry/SiDetectorDesign.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

// Space point Classes,
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerIdentifier/FaserSCT_ID.h"


// general Atlas classes
#include "FaserDetDescr/FaserDetectorID.h"
#include "xAODEventInfo/EventInfo.h"
#include "StoreGate/ReadCondHandle.h"

#include "AthenaMonitoringKernel/Monitored.h"

namespace Tracker
{
  //------------------------------------------------------------------------
  TrackerSPFit::TrackerSPFit(const std::string& name,
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
      , m_residual_y_plane0(0)
      , m_residual_y_plane1(0)
      , m_residual_y_plane2(0)
      , m_residual_y_plane3(0)
      , m_residual_y_plane4(0)
      , m_residual_y_plane5(0)
      , m_residual_y_plane6(0)
      , m_residual_y_plane7(0)
      , m_residual_y_plane8(0)
      , m_residual_x_plane0(0)
      , m_residual_x_plane1(0)
      , m_residual_x_plane2(0)
      , m_residual_x_plane3(0)
      , m_residual_x_plane4(0)
      , m_residual_x_plane5(0)
      , m_residual_x_plane6(0)
      , m_residual_x_plane7(0)
      , m_residual_x_plane8(0)
      , m_chi2(0)
      , m_edm(0)
      , m_ndf(0)
      , m_preYhistX_s1l1m0(0)
      , m_preYhistY_s1l1m0(0)
      , m_preXhistX_s1l1m0(0)
      , m_preXhistY_s1l1m0(0)
      , m_preYhistX_s1l1m1(0)
      , m_preYhistY_s1l1m1(0)
      , m_preXhistX_s1l1m1(0)
      , m_preXhistY_s1l1m1(0)
      , m_preYhistX_s1l1m2(0)
      , m_preYhistY_s1l1m2(0)
      , m_preXhistX_s1l1m2(0)
      , m_preXhistY_s1l1m2(0)
      , m_preYhistX_s1l1m3(0)
      , m_preYhistY_s1l1m3(0)
      , m_preXhistX_s1l1m3(0)
      , m_preXhistY_s1l1m3(0)
      , m_preYhistX_s1l1m4(0)
      , m_preYhistY_s1l1m4(0)
      , m_preXhistX_s1l1m4(0)
      , m_preXhistY_s1l1m4(0)
      , m_preYhistX_s1l1m5(0)
      , m_preYhistY_s1l1m5(0)
      , m_preXhistX_s1l1m5(0)
      , m_preXhistY_s1l1m5(0)
      , m_preYhistX_s1l1m6(0)
      , m_preYhistY_s1l1m6(0)
      , m_preXhistX_s1l1m6(0)
      , m_preXhistY_s1l1m6(0)
      , m_preYhistX_s1l1m7(0)
      , m_preYhistY_s1l1m7(0)
      , m_preXhistX_s1l1m7(0)
      , m_preXhistY_s1l1m7(0)
      , m_thistSvc("THistSvc", name)
  { 
    declareProperty("MaxChi2", m_maxchi2=20);
    declareProperty("UseBiasedResidual", m_bias=true);
    declareProperty("SaveAllClusterInfo", m_saveallcluster=true);
    declareProperty("SaveAllSPInfo", m_saveallsp=true);
    declareProperty("MakeDoublets", m_doublets=true);
  }

  //-----------------------------------------------------------------------
  StatusCode TrackerSPFit::initialize()
  {
    ATH_MSG_DEBUG( "TrackerSPFit::initialize()" );

    CHECK(m_thistSvc.retrieve());
    // Check that clusters, space points and ids have names
    if(m_Sct_clcontainerKey.key().empty()){                                 
      ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_Sct_clcontainerKey.initialize() );

    if ( m_Sct_spcontainerKey.key().empty()){
      ATH_MSG_FATAL( "SCTs selected and no name set for SCT clusters");
      return StatusCode::FAILURE;
    }
    ATH_CHECK( m_Sct_spcontainerKey.initialize() );

    m_tree= new TTree("spfit","tree");
    TrackerSPFit::initializeTree();

    ATH_CHECK( m_trackerSeedContainerKey.initialize() );
    if (m_trackerSeedContainerKey.key().empty()){
      ATH_MSG_FATAL( "No name set for output track seeds");
      return StatusCode::FAILURE;
    }

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
    m_residual_y_plane0=new TH1D("sp_residual_y_plane0","sp_residual_y_plane0",2000,-1,1);
    m_residual_y_plane1=new TH1D("sp_residual_y_plane1","sp_residual_y_plane1",2000,-1,1);
    m_residual_y_plane2=new TH1D("sp_residual_y_plane2","sp_residual_y_plane2",2000,-1,1);
    m_residual_y_plane3=new TH1D("sp_residual_y_plane3","sp_residual_y_plane3",2000,-1,1);
    m_residual_y_plane4=new TH1D("sp_residual_y_plane4","sp_residual_y_plane4",2000,-1,1);
    m_residual_y_plane5=new TH1D("sp_residual_y_plane5","sp_residual_y_plane5",2000,-1,1);
    m_residual_y_plane6=new TH1D("sp_residual_y_plane6","sp_residual_y_plane6",2000,-1,1);
    m_residual_y_plane7=new TH1D("sp_residual_y_plane7","sp_residual_y_plane7",2000,-1,1);
    m_residual_y_plane8=new TH1D("sp_residual_y_plane8","sp_residual_y_plane8",2000,-1,1);
    m_residual_x_plane0=new TH1D("sp_residual_x_plane0","sp_residual_x_plane0",2000,-1,1);
    m_residual_x_plane1=new TH1D("sp_residual_x_plane1","sp_residual_x_plane1",2000,-1,1);
    m_residual_x_plane2=new TH1D("sp_residual_x_plane2","sp_residual_x_plane2",2000,-1,1);
    m_residual_x_plane3=new TH1D("sp_residual_x_plane3","sp_residual_x_plane3",2000,-1,1);
    m_residual_x_plane4=new TH1D("sp_residual_x_plane4","sp_residual_x_plane4",2000,-1,1);
    m_residual_x_plane5=new TH1D("sp_residual_x_plane5","sp_residual_x_plane5",2000,-1,1);
    m_residual_x_plane6=new TH1D("sp_residual_x_plane6","sp_residual_x_plane6",2000,-1,1);
    m_residual_x_plane7=new TH1D("sp_residual_x_plane7","sp_residual_x_plane7",2000,-1,1);
    m_residual_x_plane8=new TH1D("sp_residual_x_plane8","sp_residual_x_plane8",2000,-1,1);
    m_chi2=new TH1D("chi2","chi2",100,0,100);
    m_edm=new TH1D("edm","edm",100,0,0.01);
    m_ndf=new TH1D("ndf","ndf",10,0,10);
    m_preYhistX_s1l1m0=new TProfile("YvsX_s1l1m0","residual vs x for staion 1 layer 1 module 0",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m0=new TProfile("XvsX_s1l1m0","residual vs x for staion 1 layer 1 module 0",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m0=new TProfile("XvsY_s1l1m0","residual vs x for staion 1 layer 1 module 0",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m0=new TProfile("YvsY_s1l1m0","residual vs x for staion 1 layer 1 module 0",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m1=new TProfile("YvsX_s1l1m1","residual vs x for staion 1 layer 1 module 1",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m1=new TProfile("XvsX_s1l1m1","residual vs x for staion 1 layer 1 module 1",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m1=new TProfile("XvsY_s1l1m1","residual vs x for staion 1 layer 1 module 1",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m1=new TProfile("YvsY_s1l1m1","residual vs x for staion 1 layer 1 module 1",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m2=new TProfile("YvsX_s1l1m2","residual vs x for staion 1 layer 1 module 2",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m2=new TProfile("XvsX_s1l1m2","residual vs x for staion 1 layer 1 module 2",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m2=new TProfile("XvsY_s1l1m2","residual vs x for staion 1 layer 1 module 2",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m2=new TProfile("YvsY_s1l1m2","residual vs x for staion 1 layer 1 module 2",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m3=new TProfile("YvsX_s1l1m3","residual vs x for staion 1 layer 1 module 3",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m3=new TProfile("XvsX_s1l1m3","residual vs x for staion 1 layer 1 module 3",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m3=new TProfile("XvsY_s1l1m3","residual vs x for staion 1 layer 1 module 3",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m3=new TProfile("YvsY_s1l1m3","residual vs x for staion 1 layer 1 module 3",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m4=new TProfile("YvsX_s1l1m4","residual vs x for staion 1 layer 1 module 4",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m4=new TProfile("XvsX_s1l1m4","residual vs x for staion 1 layer 1 module 4",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m4=new TProfile("XvsY_s1l1m4","residual vs x for staion 1 layer 1 module 4",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m4=new TProfile("YvsY_s1l1m4","residual vs x for staion 1 layer 1 module 4",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m5=new TProfile("YvsX_s1l1m5","residual vs x for staion 1 layer 1 module 5",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m5=new TProfile("XvsX_s1l1m5","residual vs x for staion 1 layer 1 module 5",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m5=new TProfile("XvsY_s1l1m5","residual vs x for staion 1 layer 1 module 5",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m5=new TProfile("YvsY_s1l1m5","residual vs x for staion 1 layer 1 module 5",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m6=new TProfile("YvsX_s1l1m6","residual vs x for staion 1 layer 1 module 6",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m6=new TProfile("XvsX_s1l1m6","residual vs x for staion 1 layer 1 module 6",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m6=new TProfile("XvsY_s1l1m6","residual vs x for staion 1 layer 1 module 6",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m6=new TProfile("YvsY_s1l1m6","residual vs x for staion 1 layer 1 module 6",100,-150,150,-0.1,0.1);
    m_preYhistX_s1l1m7=new TProfile("YvsX_s1l1m7","residual vs x for staion 1 layer 1 module 7",100,-150,150,-0.1,0.1);
    m_preXhistX_s1l1m7=new TProfile("XvsX_s1l1m7","residual vs x for staion 1 layer 1 module 7",100,-150,150,-0.1,0.1);
    m_preXhistY_s1l1m7=new TProfile("XvsY_s1l1m7","residual vs x for staion 1 layer 1 module 7",100,-150,150,-0.1,0.1);
    m_preYhistY_s1l1m7=new TProfile("YvsY_s1l1m7","residual vs x for staion 1 layer 1 module 7",100,-150,150,-0.1,0.1);
    CHECK(m_thistSvc->regTree("/TrackerSPFit/spfit",m_tree));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m0",m_preYhistX_s1l1m0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m0",m_preXhistX_s1l1m0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m0",m_preXhistY_s1l1m0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m0",m_preYhistY_s1l1m0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m1",m_preYhistX_s1l1m1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m1",m_preXhistX_s1l1m1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m1",m_preXhistY_s1l1m1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m1",m_preYhistY_s1l1m1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m2",m_preYhistX_s1l1m2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m2",m_preXhistX_s1l1m2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m2",m_preXhistY_s1l1m2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m2",m_preYhistY_s1l1m2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m3",m_preYhistX_s1l1m3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m3",m_preXhistX_s1l1m3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m3",m_preXhistY_s1l1m3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m3",m_preYhistY_s1l1m3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m4",m_preYhistX_s1l1m4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m4",m_preXhistX_s1l1m4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m4",m_preXhistY_s1l1m4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m4",m_preYhistY_s1l1m4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m5",m_preYhistX_s1l1m5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m5",m_preXhistX_s1l1m5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m5",m_preXhistY_s1l1m5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m5",m_preYhistY_s1l1m5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m6",m_preYhistX_s1l1m6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m6",m_preXhistX_s1l1m6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m6",m_preXhistY_s1l1m6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m6",m_preYhistY_s1l1m6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_x_s1l1m7",m_preYhistX_s1l1m7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_x_s1l1m7",m_preXhistX_s1l1m7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualX_y_s1l1m7",m_preXhistY_s1l1m7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residualY_y_s1l1m7",m_preYhistY_s1l1m7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x",m_hist_x));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_y",m_hist_y));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_z",m_hist_z));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_r",m_hist_r));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_eta",m_hist_eta));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_phi",m_hist_phi));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_strip",m_hist_strip));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_layer",m_hist_layer));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_station",m_hist_station));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane0",m_hist_x_y_plane0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane1",m_hist_x_y_plane1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane2",m_hist_x_y_plane2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane3",m_hist_x_y_plane3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane4",m_hist_x_y_plane4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane5",m_hist_x_y_plane5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane6",m_hist_x_y_plane6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane7",m_hist_x_y_plane7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_x_y_plane8",m_hist_x_y_plane8));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane0",m_residual_y_plane0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane1",m_residual_y_plane1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane2",m_residual_y_plane2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane3",m_residual_y_plane3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane4",m_residual_y_plane4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane5",m_residual_y_plane5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane6",m_residual_y_plane6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane7",m_residual_y_plane7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_y_plane8",m_residual_y_plane8));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane0",m_residual_x_plane0));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane1",m_residual_x_plane1));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane2",m_residual_x_plane2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane3",m_residual_x_plane3));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane4",m_residual_x_plane4));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane5",m_residual_x_plane5));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane6",m_residual_x_plane6));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane7",m_residual_x_plane7));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_residual_x_plane8",m_residual_x_plane8));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_chi2",m_chi2));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_edm",m_edm));
    CHECK(m_thistSvc->regHist("/TrackerSPFit/sp/sp_ndf",m_ndf));
    ATH_MSG_INFO( "TrackerSPFit::initialized" );
    return StatusCode::SUCCESS;
  }

  //-------------------------------------------------------------------------
  void TrackerSPFit::initializeTree(){
    m_tree->Branch("evtId",&m_eventNumber);
    m_tree->Branch("sp_track_x",&m_sp_track_x);
    m_tree->Branch("sp_track_y",&m_sp_track_y);
    m_tree->Branch("sp_track_z",&m_sp_track_z);
    m_tree->Branch("sp_track_x_err",&m_sp_track_x_err);
    m_tree->Branch("sp_track_y_err",&m_sp_track_y_err);
    m_tree->Branch("sp_track_z_err",&m_sp_track_z_err);
    m_tree->Branch("sp_track_x_predicted",&m_sp_track_x_predicted);
    m_tree->Branch("sp_track_y_predicted",&m_sp_track_y_predicted);
    m_tree->Branch("sp_track_z_predicted",&m_sp_track_z_predicted);
    m_tree->Branch("sp_track_x_residual",&m_sp_track_x_residual);
    m_tree->Branch("sp_track_y_residual",&m_sp_track_y_residual);
    m_tree->Branch("sp_track_z_residual",&m_sp_track_z_residual);
    m_tree->Branch("sp_track_station",&m_sp_track_station);
    m_tree->Branch("sp_track_layer",&m_sp_track_layer);
    m_tree->Branch("sp_track_module",&m_sp_track_module);
    m_tree->Branch("trackId",&m_trackId);
    m_tree->Branch("track_chi2",&m_track_chi2);
    m_tree->Branch("track_edm",&m_track_edm);
    m_tree->Branch("track_ndf",&m_track_ndf);
    m_tree->Branch("track_p0",&m_track_p0);
    m_tree->Branch("track_p1",&m_track_p1);
    m_tree->Branch("track_p2",&m_track_p2);
    m_tree->Branch("track_p3",&m_track_p3);
    if(m_saveallcluster){
      m_tree->Branch("cluster_all_identify",&m_cluster_all_identify);
      m_tree->Branch("cluster_all_local_x",&m_cluster_all_local_x);
      m_tree->Branch("cluster_all_local_y",&m_cluster_all_local_y);
      m_tree->Branch("cluster_global_x",&m_cluster_all_global_x);
      m_tree->Branch("cluster_global_y",&m_cluster_all_global_y);
      m_tree->Branch("cluster_global_z",&m_cluster_all_global_z);
      m_tree->Branch("cluster_all_phiwidth",&m_cluster_all_phiwidth);
      m_tree->Branch("cluster_all_size",&m_cluster_all_size);
      m_tree->Branch("cluster_all_station",&m_cluster_all_station);
      m_tree->Branch("cluster_all_layer",&m_cluster_all_layer);
      m_tree->Branch("cluster_all_module",&m_cluster_all_module);
      m_tree->Branch("cluster_all_side",&m_cluster_all_side);
      m_tree->Branch("cluster_all_strip",&m_cluster_all_strip);
    }
    if(m_saveallsp){
      m_tree->Branch("sp_all_identify0", &m_sp_all_identify0);
      m_tree->Branch("sp_all_identify1", &m_sp_all_identify1);
      m_tree->Branch("sp_all_x_local", &m_sp_all_x_local);
      m_tree->Branch("sp_all_y_local", &m_sp_all_y_local);
      m_tree->Branch("sp_all_x", &m_sp_all_x);
      m_tree->Branch("sp_all_y", &m_sp_all_y);
      m_tree->Branch("sp_all_z", &m_sp_all_z);
      m_tree->Branch("sp_all_station", &m_sp_all_station);
      m_tree->Branch("sp_all_layer", &m_sp_all_layer);
      m_tree->Branch("sp_all_module", &m_sp_all_module);
      //      if(m_saveallcluster){
      //	m_tree->Branch("sp_all_clus1_index", &m_sp_all_clus1_index);
      //	m_tree->Branch("sp_all_clus2_index", &m_sp_all_clus2_index);
      //      }
    }
  }
  //-------------------------------------------------------------------------
  void TrackerSPFit::clearVariables() const {
    if(m_saveallcluster){
      m_cluster_all_identify.clear();
      m_cluster_all_local_x.clear();
      m_cluster_all_local_y.clear();
      m_cluster_all_global_x.clear();
      m_cluster_all_global_y.clear();
      m_cluster_all_global_z.clear();
      m_cluster_all_phiwidth.clear();
      m_cluster_all_size.clear();
      m_cluster_all_station.clear();
      m_cluster_all_layer.clear();
      m_cluster_all_module.clear();
      m_cluster_all_side.clear();
      m_cluster_all_strip.clear();
    }
    if(m_saveallsp){
      m_sp_all_identify0.clear();
      m_sp_all_identify1.clear();
      m_sp_all_x_local.clear();
      m_sp_all_y_local.clear();
      m_sp_all_x.clear();
      m_sp_all_y.clear();
      m_sp_all_z.clear();
      //      if(m_saveallcluster){
      //	m_sp_all_clus1_index.clear();
      //	m_sp_all_clus2_index.clear();
      //      }
      m_sp_all_station.clear();
      m_sp_all_layer.clear();
      m_sp_all_module.clear();
    }
    m_sp_track_x.clear();
    m_sp_track_y.clear();
    m_sp_track_z.clear();
    m_sp_track_x_err.clear();
    m_sp_track_y_err.clear();
    m_sp_track_z_err.clear();
    m_sp_track_x_predicted.clear();
    m_sp_track_y_predicted.clear();
    m_sp_track_z_predicted.clear();
    m_sp_track_x_residual.clear();
    m_sp_track_y_residual.clear();
    m_sp_track_z_residual.clear();
    m_sp_track_station.clear();
    m_sp_track_layer.clear();
    m_sp_track_module.clear();
    m_trackId.clear();
    m_track_chi2.clear();
    m_track_edm.clear();
    m_track_ndf.clear();
    m_track_p0.clear();
    m_track_p1.clear();
    m_track_p2.clear();
    m_track_p3.clear();
  }
  //-------------------------------------------------------------------------

  StatusCode TrackerSPFit::execute (const EventContext& ctx) const
  {

    ++m_numberOfEvents;
    m_eventNumber=m_numberOfEvents;
    clearVariables();
    const TrackerDD::SiDetectorElementCollection* elements = nullptr;


    SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> sctDetEle(m_SCTDetEleCollKey, ctx);
    elements = sctDetEle.retrieve();
    if (elements==nullptr) {
      ATH_MSG_FATAL("Pointer of SiDetectorElementCollection (" << m_SCTDetEleCollKey.fullKey() << ") could not be retrieved");
      return StatusCode::SUCCESS;
    }


    SG::WriteHandle<TrackerSeedCollection> seedContainer(m_trackerSeedContainerKey, ctx);
    ATH_CHECK(seedContainer.record( std::make_unique<TrackerSeedCollection>() ) );
    ATH_MSG_DEBUG("Created track seed container " << m_trackerSeedContainerKey.key());


    //read cluster
    SG::ReadHandle<Tracker::FaserSCT_ClusterContainer> sct_clcontainer( m_Sct_clcontainerKey, ctx );
    if ((!sct_clcontainer.isValid())&&m_saveallcluster){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_clcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }
    // save all the cluster information
    if(m_saveallcluster){
      ATH_MSG_DEBUG( "SCT cluster container found: " << sct_clcontainer->size() << " collections" );
      Tracker::FaserSCT_ClusterContainer::const_iterator clusit = sct_clcontainer->begin();
      Tracker::FaserSCT_ClusterContainer::const_iterator clusend = sct_clcontainer->end();
      for (; clusit != clusend; ++clusit){
	++m_numberOfClusterCol;
	const Tracker::FaserSCT_ClusterCollection *colNext=&(**clusit);
	FaserSCT_ClusterCollection::const_iterator clusters {colNext->begin()}    ;
	FaserSCT_ClusterCollection::const_iterator clustersEnd {colNext->end()    };
	  ATH_MSG_DEBUG("loop over all clusters " );
	for (; clusters != clustersEnd; ++clusters)
	{
	  ++m_numberOfCluster;
	  const Tracker::FaserSCT_Cluster* clus=&(**clusters);
	  m_cluster_all_global_x.push_back(clus->globalPosition().x());
	  m_cluster_all_global_y.push_back(clus->globalPosition().y());
	  m_cluster_all_global_z.push_back(clus->globalPosition().z());
	  m_cluster_all_local_x.push_back(clus->localPosition().x());
	  m_cluster_all_local_y.push_back(clus->localPosition().y());
	  m_cluster_all_phiwidth.push_back(clus->width().phiR());
	  Identifier elementID = clus->identify();
	  m_cluster_all_station.push_back( m_idHelper->station(elementID));
	  m_cluster_all_identify.push_back( elementID.get_compact());
	  m_cluster_all_layer.push_back( m_idHelper->layer(elementID));
	  int ietamodule = m_idHelper->eta_module(elementID);
	  int iphimodule = m_idHelper->phi_module(elementID);
	  if(ietamodule<0)iphimodule+=4;
	  m_cluster_all_module.push_back(iphimodule);
	  m_cluster_all_side.push_back(m_idHelper->side(elementID));
	  m_cluster_all_strip.push_back(m_idHelper->strip(elementID));
	  m_cluster_all_size.push_back(clus->rdoList().size());
	  ATH_MSG_DEBUG("cluster found in station/layer/phi/etamodule "<<m_idHelper->station(elementID)<<"/"<<m_idHelper->layer(elementID)<<"/"<<ietamodule<<"/"<<iphimodule );
	}
      }

    }


    // retrieve SCT spacepoint container
    SG::ReadHandle<FaserSCT_SpacePointContainer> sct_spcontainer{ m_Sct_spcontainerKey, ctx };
    if (!sct_spcontainer.isValid()){
      msg(MSG:: FATAL) << "Could not find the data object "<< sct_spcontainer.name() << " !" << endmsg;
      return StatusCode::RECOVERABLE;
    }

    // save all the sp information
    int nsp_sta[4] = {0};
    if(m_saveallsp){
      ATH_MSG_DEBUG( "SCT spacepoint container found: " << sct_spcontainer->size() << " collections" );
      FaserSCT_SpacePointContainer::const_iterator it = sct_spcontainer->begin();
      FaserSCT_SpacePointContainer::const_iterator itend = sct_spcontainer->end();

      for (; it != itend; ++it){
	++m_numberOfSPCol;
	ATH_MSG_DEBUG( "loop over spacepoint collection " << m_numberOfSPCol );
	const FaserSCT_SpacePointCollection *colNext=&(**it);
	FaserSCT_SpacePointCollection::const_iterator sp_begin= colNext->begin();
	FaserSCT_SpacePointCollection::const_iterator sp_end= colNext->end();
	Identifier elementID = colNext->identify();
	  ATH_MSG_DEBUG("loop over all spacepoint in collection with size = "<<colNext->size());
	for (; sp_begin != sp_end; ++sp_begin){
	  ++m_numberOfSP;
	  ATH_MSG_DEBUG("reading "<<m_numberOfSP<<" Spacepoint at station "<<m_idHelper->station(elementID));
	  const Tracker::FaserSCT_SpacePoint* sp=&(**sp_begin);
	  m_sp_all_x_local.push_back(sp->localParameters().x());
	  m_sp_all_y_local.push_back(sp->localParameters().y());
	  m_sp_all_x.push_back(sp->globalPosition().x());
	  m_sp_all_y.push_back(sp->globalPosition().y());
	  m_sp_all_z.push_back(sp->globalPosition().z());
	  m_sp_all_station.push_back(m_idHelper->station(elementID));
	  nsp_sta[m_idHelper->station(elementID)] += 1;
	  m_sp_all_layer.push_back(m_idHelper->layer(elementID));
	  m_sp_all_identify0.push_back(sp->clusterList().first->identify().get_compact());
	  m_sp_all_identify1.push_back(sp->clusterList().second->identify().get_compact());
	  int ietamodule = m_idHelper->eta_module(elementID);
	  int iphimodule = m_idHelper->phi_module(elementID);
	  if(ietamodule<0)iphimodule+=4;
	  m_sp_all_module.push_back(iphimodule);
	  ATH_MSG_DEBUG( "SCT spacepoint found: " << sp->globalPosition().x()<<" , "<<sp->globalPosition().y()<<" , "<<sp->globalPosition().z()<<" , "<<m_idHelper->station(elementID)<<" , "<<m_idHelper->layer(elementID)<<" , "<<m_idHelper->eta_module(elementID)<<" , "<<m_idHelper->phi_module(elementID) );
	  /* to be updated
	     if(m_saveallcluster){
	     const Tracker::FaserSCT_ClusterCollection *ptr1 = sct_clcontainer->indexFindPtr(sp->elemIDList().first());
	     const Tracker::FaserSCT_ClusterCollection *ptr2 = sct_clcontainer->indexFindPtr(sp->elemIDList().second());
	     m_sp_all_clus1_index.push_back(findClusterIndex(sp,ptr1));
	     m_sp_all_clus2_index.push_back(findClusterIndex(sp,ptr2));
	     }
	     */
	}
      }
    }

    // register the IdentifiableContainer into StoreGate

    ATH_MSG_DEBUG( "SCT spacepoint container found: " << sct_spcontainer->size() << " collections" );
    FaserSCT_SpacePointContainer::const_iterator it = sct_spcontainer->begin();
    FaserSCT_SpacePointContainer::const_iterator itend = sct_spcontainer->end();

    std::vector<std::vector<SP_Seed>> sp_sta;
    sp_sta.resize(4);
    sp_sta[0].resize(nsp_sta[0]);
    sp_sta[1].resize(nsp_sta[1]);
    sp_sta[2].resize(nsp_sta[2]);
    sp_sta[3].resize(nsp_sta[3]);
    int index_sp_sta[4] = {0};
    for (; it != itend; ++it){
      const FaserSCT_SpacePointCollection *colNext=&(**it);
      // int nReceivedSPSCT = colNext->size();

      // Create SpacePointCollection
      // IdentifierHash idHash = colNext->identifyHash();
      Identifier elementID = colNext->identify();

      ATH_MSG_VERBOSE("reading SpacePoints collection "<<elementID.get_compact()<<" , and hash = "<<colNext->identifyHash() );
      int istation = m_idHelper->station(elementID);
//      if(istation !=0) continue;
      int ilayer = m_idHelper->layer(elementID);
      int ietamodule = m_idHelper->eta_module(elementID);
      int iphimodule = m_idHelper->phi_module(elementID);
      int imodule=iphimodule;
      if(ietamodule<0)imodule+=4;
      m_hist_strip->Fill(m_idHelper->strip(elementID));
      m_hist_station->Fill(istation);
      m_hist_layer->Fill(ilayer);
      size_t size = colNext->size();
      if (size == 0){
	ATH_MSG_DEBUG( "TrackerSPFit algorithm found no space points" );
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
	  if(gloPos.x()>150||gloPos.x()<-150||gloPos.y()>150||gloPos.y()<-150||gloPos.z()==0)continue;
	  if(sqrt(gloPos.x()*gloPos.x()+gloPos.y()*gloPos.y()+gloPos.z()*gloPos.z())<0.1)continue;
	  m_hist_x->Fill(gloPos.x());
	  m_hist_y->Fill(gloPos.y());
	  m_hist_z->Fill(gloPos.z());
	  struct SP_Seed tmp{gloPos,sp->globCovariance(),ilayer,imodule};
	  sp_sta[istation][index_sp_sta[istation]]=tmp;
	  index_sp_sta[istation] += 1;

	  if ( ((istation)*3+ilayer) == 0 ) m_hist_x_y_plane0->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 1 ) m_hist_x_y_plane1->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 2 ) m_hist_x_y_plane2->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 3 ) m_hist_x_y_plane3->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 4 ) m_hist_x_y_plane4->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 5 ) m_hist_x_y_plane5->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 6 ) m_hist_x_y_plane6->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 7 ) m_hist_x_y_plane7->Fill(gloPos.x(),gloPos.y());
	  if ( ((istation)*3+ilayer) == 8 ) m_hist_x_y_plane8->Fill(gloPos.x(),gloPos.y());
	}
	ATH_MSG_VERBOSE( size << " SpacePoints successfully added to Container !" );
      }
    }
    ATH_MSG_INFO( "TrackerSPFit number of spacepoints in each stations: "<<sp_sta[0].size()<<" "<<sp_sta[1].size()<<" "<<sp_sta[2].size() );

    bool ifTrack=false;
    for(int ista=0;ista<4;ista++){
      if(sp_sta[ista].size()>1&&sp_sta[ista].size()<100) {
	auto tracks_sta0=makeTrackSeg(sp_sta[ista],m_maxchi2);
	ATH_MSG_INFO( "TrackerSPFit found "<<tracks_sta0.size()<<" track segments in station "<<ista );
	if(tracks_sta0.size()>0){
	  ifTrack=true;
	  ATH_MSG_DEBUG( "TrackerSPFit fill the track info0 " );

	  //make TrackerSeed
	  if(m_bias){
	    for(size_t iseed=0;iseed<tracks_sta0.size();iseed+=3){
	      Tracker::TrackerSeed* trackerSeed = new Tracker::TrackerSeed();
	      trackerSeed->setStation(ista);
	      trackerSeed->set_id(TrackerSeed::TRIPLET_SP);
	      double ndf=tracks_sta0[iseed].ndf;
	      double* param=new double[6];
	      param[0] = tracks_sta0[iseed].p0;
	      param[1] = tracks_sta0[iseed].p1;
	      param[2] = tracks_sta0[iseed].p2;
	      param[3] = tracks_sta0[iseed].p3;
	      param[4] = tracks_sta0[iseed].chi2;
	      param[5] = ndf;
	      //	    double* param[] = {tracks_sta0[iseed].p0, tracks_sta0[iseed].p1, tracks_sta0[iseed].p2, tracks_sta0[iseed].p3, tracks_sta0[iseed].chi2, ndf};
	      trackerSeed->setParameters(param);
	      std::vector<const Tracker::FaserSCT_SpacePoint*> spseed;
	      spseed.clear();
	      spseed.emplace_back(findSpacePoint(&*sct_spcontainer, tracks_sta0[iseed].pos.x(), tracks_sta0[iseed].pos.y(), tracks_sta0[iseed].pos.z()));
	      spseed.emplace_back(findSpacePoint(&*sct_spcontainer, tracks_sta0[iseed+1].pos.x(), tracks_sta0[iseed+1].pos.y(), tracks_sta0[iseed+1].pos.z()));
	      spseed.emplace_back(findSpacePoint(&*sct_spcontainer, tracks_sta0[iseed+2].pos.x(), tracks_sta0[iseed+2].pos.y(), tracks_sta0[iseed+2].pos.z()));
	      trackerSeed->add(spseed);
	      seedContainer->push_back(trackerSeed);
	    }

	  }

	  for(auto track:tracks_sta0){
	    auto pre=track.predicted;
	    auto pos=track.pos;
	    ++m_numberOfTrack;
	    m_trackId.push_back(int(m_numberOfTrack/3));
	    m_sp_track_x.push_back(pos.x());
	    m_sp_track_y.push_back(pos.y());
	    m_sp_track_z.push_back(pos.z());
	    m_sp_track_x_err.push_back(track.err.x());
	    m_sp_track_y_err.push_back(track.err.y());
	    m_sp_track_z_err.push_back(track.err.z());
	    m_sp_track_x_predicted.push_back(pre.x());
	    m_sp_track_y_predicted.push_back(pre.y());
	    m_sp_track_z_predicted.push_back(pre.z());
	    m_sp_track_x_residual.push_back(pre.x()-pos.x());
	    m_sp_track_y_residual.push_back(pre.y()-pos.y());
	    m_sp_track_z_residual.push_back(pre.z()-pos.z());
	    ATH_MSG_DEBUG(" fill the result "<<pre.x()<<" "<<pre.y()<<"  "<<pre.z()<<" "<<pos.x()<<" "<<pos.y()<<" "<<pos.z());
	    ATH_MSG_DEBUG(" Station/layer/module = "<<ista<<"/"<<track.layer<<"/"<<track.module<<" , and track ID "<<m_numberOfTrack/3);
	    m_sp_track_station.push_back(0);
	    m_sp_track_layer.push_back(track.layer);
	    m_sp_track_module.push_back(track.module);

	    if(m_bias){
	      m_track_chi2.push_back(track.chi2);
	      m_track_edm.push_back(track.edm);
	      m_track_ndf.push_back(track.ndf);
	      m_track_p0.push_back(track.p0);
	      m_track_p1.push_back(track.p1);
	      m_track_p2.push_back(track.p2);
	      m_track_p3.push_back(track.p3);

	    }
	    if(track.layer==0){ m_residual_x_plane0->Fill(pre.x()-pos.x());m_residual_y_plane0->Fill(pre.y()-pos.y());}
	    if(track.layer==1){ m_residual_x_plane1->Fill(pre.x()-pos.x());m_residual_y_plane1->Fill(pre.y()-pos.y());}
	    if(track.layer==2){ m_residual_x_plane2->Fill(pre.x()-pos.x());m_residual_y_plane2->Fill(pre.y()-pos.y());}
	    if(track.layer==1){
	      ATH_MSG_DEBUG(" only fill the layer 1, module =  "<<track.module);
	      ATH_MSG_DEBUG(" measured =  "<<pos.x()<<" , "<<pos.y()<<" , "<<pos.z());
	      ATH_MSG_DEBUG(" predicted =  "<<pre.x()<<" , "<<pre.y()<<" , "<<pre.z());
	      if(track.module==0){
		m_preYhistX_s1l1m0->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m0->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m0->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m0->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==1){
		m_preYhistX_s1l1m1->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m1->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m1->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m1->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==2){
		m_preYhistX_s1l1m2->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m2->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m2->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m2->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==3){
		m_preYhistX_s1l1m3->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m3->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m3->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m3->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==4){
		m_preYhistX_s1l1m4->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m4->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m4->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m4->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==5){
		m_preYhistX_s1l1m5->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m5->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m5->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m5->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==6){
		m_preYhistX_s1l1m6->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m6->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m6->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m6->Fill(pos.y(),pos.x()-pre.x());
	      }
	      if(track.module==7){
		m_preYhistX_s1l1m7->Fill(pos.x(),pos.y()-pre.y());
		m_preYhistY_s1l1m7->Fill(pos.y(),pos.y()-pre.y());
		m_preXhistX_s1l1m7->Fill(pos.x(),pos.x()-pre.x());
		m_preXhistY_s1l1m7->Fill(pos.y(),pos.x()-pre.x());
	      }
	    }
	  }
	}
	else if(m_doublets){
	  for(unsigned int isp=0;isp<sp_sta[ista].size()-1;isp++){
	    for(unsigned int jsp=isp+1;jsp<sp_sta[ista].size();jsp++){
	      if(sp_sta[ista][isp].layer != sp_sta[ista][jsp].layer){
		Tracker::TrackerSeed* trackerSeed = new Tracker::TrackerSeed();
		trackerSeed->setStation(ista);
		trackerSeed->set_id(TrackerSeed::DOUBLET_SP);
		double* param = new double[6];
		double tmp_x_slope = (sp_sta[ista][jsp].pos.x()-sp_sta[ista][isp].pos.x())/(sp_sta[ista][jsp].pos.z()-sp_sta[ista][isp].pos.z());
		double tmp_x_offset = sp_sta[ista][isp].pos.x()-sp_sta[ista][isp].pos.x()*tmp_x_slope;
		double tmp_y_slope = (sp_sta[ista][jsp].pos.y()-sp_sta[ista][isp].pos.y())/(sp_sta[ista][jsp].pos.z()-sp_sta[ista][isp].pos.z());
		double tmp_y_offset = sp_sta[ista][isp].pos.y()-sp_sta[ista][isp].pos.z()*tmp_y_slope;
		param[0] = tmp_x_offset;
		param[1] = tmp_x_slope;
		param[2] = tmp_y_offset;
		param[3] = tmp_y_slope;
		param[4] = -999;
		param[5] = -999;
		trackerSeed->setParameters(param);
		std::vector<const Tracker::FaserSCT_SpacePoint*> spseed;
		spseed.clear();
		spseed.emplace_back(findSpacePoint(&*sct_spcontainer, sp_sta[ista][isp].pos.x(), sp_sta[ista][isp].pos.y(), sp_sta[ista][isp].pos.z()));
		spseed.emplace_back(findSpacePoint(&*sct_spcontainer, sp_sta[ista][jsp].pos.x(), sp_sta[ista][jsp].pos.y(), sp_sta[ista][jsp].pos.z()));
		trackerSeed->add(spseed);
		seedContainer->push_back(trackerSeed);
	      }
	    }
	  }
	}
      }
    }

    if(ifTrack||m_saveallcluster||m_saveallsp)
      m_tree->Fill();

    return StatusCode::SUCCESS;
  }

  //---------------------------------------------------------------------------
  StatusCode TrackerSPFit::finalize()
  {
    ATH_MSG_INFO( "TrackerSPFit::finalize()" );
    ATH_MSG_INFO( m_numberOfEvents << " events processed" );
    ATH_MSG_INFO( m_numberOfClusterCol<< " sct Cluster collections processed" );
    ATH_MSG_INFO( m_numberOfCluster<< " sct Cluster processed" );
    ATH_MSG_INFO( m_numberOfSPCol<< " sct SP collections processed" );
    ATH_MSG_INFO( m_numberOfSP<< " sct SP processed" );


    //do a fitting
    TF1* fpol1=new TF1("fpol1","pol1(0)");
    m_preYhistX_s1l1m0->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 0 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m1->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 1 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m2->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 2 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m3->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 3 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m4->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 4 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m5->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 5 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m6->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 6 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    m_preYhistX_s1l1m7->Fit(fpol1);
    ATH_MSG_INFO("Fitting the residualY vs X for module 7 : parameter = "<<fpol1->GetParameter(0)<<" "<<fpol1->GetParameter(1));
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------
  std::vector<TrackerSPFit::SP_TSOS> TrackerSPFit::makeTrackSeg(std::vector<SP_Seed> sps, double maxchi2) const {
    std::vector<SP_Seed> layer0;
    std::vector<SP_Seed> layer1;
    std::vector<SP_Seed> layer2;
    layer0.clear();layer1.clear();layer2.clear();
    std::vector<SP_TSOS> tracks;
    tracks.clear();
    for(unsigned int isp=0;isp<sps.size();isp++){
      if(sps[isp].layer==0)layer0.push_back(sps[isp]);
      if(sps[isp].layer==1)layer1.push_back(sps[isp]);
      if(sps[isp].layer==2)layer2.push_back(sps[isp]);
    }
    if(layer0.size()>0&&layer1.size()>0&&layer2.size()>0){
      for(unsigned int i0=0;i0<layer0.size();i0++){
	for(unsigned int i1=0;i1<layer1.size();i1++){
	  for(unsigned int i2=0;i2<layer2.size();i2++){
	    auto tmp_bias=makeTrackSeg(layer0[i0],layer1[i1],layer2[i2],maxchi2);
	    if(tmp_bias.size()>0){
	      if(m_bias){
		//for biased residual
		tracks.insert(tracks.end(),tmp_bias.begin(),tmp_bias.end());
	      }
	      else{
		//for un-biased residual
		auto tmp=makeTrackSeg(layer0[i0],layer1[i1],layer2[i2]);
		tracks.insert(tracks.end(),tmp.begin(),tmp.end());
	      }
	    }
	  }
	}
      }
    }
    return tracks;
  }

  std::vector<TrackerSPFit::SP_TSOS> TrackerSPFit::makeTrackSeg(SP_Seed sp0, SP_Seed sp1, SP_Seed sp2) const{
    std::vector<SP_TSOS> spt;
    spt.clear();
    for(int isp=0;isp<3;isp++){
      Amg::Vector3D error;

      if(isp==0){
	error=Amg::Vector3D(sqrt((sp0.cov)(0,0)),sqrt((sp0.cov)(1,1)),sqrt((sp0.cov)(2,2)));
	//double resx= sp0.pos.x()-(sp1.pos.x()+sp2.pos.x())/2.;//from Jamie's code
	double posx= (sp1.pos.x()-((sp1.pos.x()-sp2.pos.x())*(sp1.pos.z()-sp0.pos.z())/(sp1.pos.z()-sp2.pos.z())));
	double posy= (sp1.pos.y()-((sp1.pos.y()-sp2.pos.y())*(sp1.pos.z()-sp0.pos.z())/(sp1.pos.z()-sp2.pos.z())));
	Amg::Vector3D predictedpoint2(posx,posy,sp0.pos.z());

	struct SP_TSOS tsos0{sp0.pos,predictedpoint2,error,sp0.layer,sp0.module};
	spt.push_back(tsos0);
      }
      else if(isp==1){
	error=Amg::Vector3D(sqrt((sp1.cov)(0,0)),sqrt((sp1.cov)(1,1)),sqrt((sp1.cov)(2,2)));
	double posx= (sp2.pos.x()-((sp2.pos.x()-sp0.pos.x())*(sp2.pos.z()-sp1.pos.z())/(sp2.pos.z()-sp0.pos.z())));
	double posy= (sp2.pos.y()-((sp2.pos.y()-sp0.pos.y())*(sp2.pos.z()-sp1.pos.z())/(sp2.pos.z()-sp0.pos.z())));
	Amg::Vector3D predictedpoint2(posx,posy,sp1.pos.z());

	struct SP_TSOS tsos0{sp1.pos,predictedpoint2,error,sp1.layer,sp1.module};
	spt.push_back(tsos0);
      }
      else{
	error=Amg::Vector3D(sqrt((sp2.cov)(0,0)),sqrt((sp2.cov)(1,1)),sqrt((sp2.cov)(2,2)));
	double posx= (sp1.pos.x()-((sp1.pos.x()-sp0.pos.x())*(sp1.pos.z()-sp2.pos.z())/(sp1.pos.z()-sp0.pos.z())));
	double posy= (sp1.pos.y()-((sp1.pos.y()-sp0.pos.y())*(sp1.pos.z()-sp2.pos.z())/(sp1.pos.z()-sp0.pos.z())));
	Amg::Vector3D predictedpoint2(posx,posy,sp2.pos.z());

	struct SP_TSOS tsos0{sp2.pos,predictedpoint2,error,sp2.layer,sp2.module};
	spt.push_back(tsos0);
      }

    }
    return spt;
  }
  std::vector<TrackerSPFit::SP_TSOS> TrackerSPFit::makeTrackSeg(SP_Seed sp0, SP_Seed sp1, SP_Seed sp2,  double maxchi2) const{
    std::vector<SP_TSOS> spt;
    spt.clear();
    TGraph2DErrors* gra= new TGraph2DErrors();
    gra->SetPoint(0,sp0.pos.x(),sp0.pos.y(),sp0.pos.z());
    gra->SetPoint(1,sp1.pos.x(),sp1.pos.y(),sp1.pos.z());
    gra->SetPoint(2,sp2.pos.x(),sp2.pos.y(),sp2.pos.z());
    gra->SetPointError(0,sqrt((sp0.cov)(0,0)),sqrt((sp0.cov)(1,1)),sqrt((sp0.cov)(2,2)));
    gra->SetPointError(1,sqrt((sp1.cov)(0,0)),sqrt((sp1.cov)(1,1)),sqrt((sp1.cov)(2,2)));
    gra->SetPointError(2,sqrt((sp2.cov)(0,0)),sqrt((sp2.cov)(1,1)),sqrt((sp2.cov)(2,2)));
    // ATH_MSG_DEBUG(" spacepoints in the station");
    // ATH_MSG_DEBUG("sp 0 "<<sp0.pos.x()<<" "<<sp0.pos.y()<<" "<<sp0.pos.z()<<" "<<sqrt((sp0.cov)(0,0))<<" "<<sqrt((sp0.cov)(1,1))<<" "<<sqrt((sp0.cov)(2.2)));
    // ATH_MSG_DEBUG("sp 1 "<<sp1.pos.x()<<" "<<sp1.pos.y()<<" "<<sp1.pos.z()<<" "<<sqrt((sp1.cov)(0,0))<<" "<<sqrt((sp1.cov)(1,1))<<" "<<sqrt((sp1.cov)(2.2)));
    // ATH_MSG_DEBUG("sp 2 "<<sp2.pos.x()<<" "<<sp2.pos.y()<<" "<<sp2.pos.z()<<" "<<sqrt((sp2.cov)(0,0))<<" "<<sqrt((sp2.cov)(1,1))<<" "<<sqrt((sp2.cov)(2.2)));
    ROOT::Fit::Fitter fitter;
    SumDistance2 sumdist(gra);
    ROOT::Math::Functor fcn(sumdist,4);
    double initParam[4]={1,1,1,1};
    fitter.SetFCN(fcn,initParam);
    //   for (int i = 0; i < 4; ++i) fitter.Config().ParSettings(i).SetStepSize(0.01);
    if(fitter.FitFCN()){
      const ROOT::Fit::FitResult & result =fitter.Result();
      double chi2=result.MinFcnValue();
      double edm=result.Edm();
      int ndf=result.Ndf();
      ATH_MSG_DEBUG( "TrackerSPFit::makeTrackSeg(), track chi2 = "<<chi2<<"  ; edm = "<<edm<<" ; ndf = "<<ndf );
      m_chi2->Fill(chi2);
      m_edm->Fill(edm);
      m_ndf->Fill(ndf);

      if(chi2<maxchi2){
	const double *fitParam=result.GetParams();
	ATH_MSG_DEBUG(" fit status: ook "<<chi2<<" "<<edm<<" "<<ndf<<" "<<fitParam[0]<<" "<<fitParam[1]<<" "<<fitParam[2]<<" "<<fitParam[3]);
	Amg::Vector3D err0(sqrt((sp0.cov)(0,0)),sqrt((sp0.cov)(1,1)),sqrt((sp0.cov)(2,2)));
	Amg::Vector3D err1(sqrt((sp1.cov)(0,0)),sqrt((sp1.cov)(1,1)),sqrt((sp1.cov)(2,2)));
	Amg::Vector3D err2(sqrt((sp2.cov)(0,0)),sqrt((sp2.cov)(1,1)),sqrt((sp2.cov)(2,2)));
	struct SP_TSOS tsos0{sp0.pos,predicted(sp0.pos.z(),fitParam),err0,sp0.layer,sp0.module,chi2,edm,ndf,fitParam[0],fitParam[1],fitParam[2],fitParam[3]};
	struct SP_TSOS tsos1{sp1.pos,predicted(sp1.pos.z(),fitParam),err1,sp1.layer,sp0.module,chi2,edm,ndf,fitParam[0],fitParam[1],fitParam[2],fitParam[3]};
	struct SP_TSOS tsos2{sp2.pos,predicted(sp2.pos.z(),fitParam),err2,sp2.layer,sp0.module,chi2,edm,ndf,fitParam[0],fitParam[1],fitParam[2],fitParam[3]};
	spt.push_back(tsos0);
	spt.push_back(tsos1);
	spt.push_back(tsos2);
      }
    }

    return spt;
  }

  Amg::Vector3D TrackerSPFit::predicted(double z, const double *p) const {
    return Amg::Vector3D(p[0]+p[1]*z,p[2]+p[3]*z,z);
  }

  const FaserSCT_SpacePoint* TrackerSPFit::findSpacePoint(const FaserSCT_SpacePointContainer* spcontainer, double x, double y, double z) const{
    FaserSCT_SpacePointContainer::const_iterator it = spcontainer->begin();
    FaserSCT_SpacePointContainer::const_iterator itend = spcontainer->end();

    for (; it != itend; ++it){
      const FaserSCT_SpacePointCollection *colNext=&(**it);
      FaserSCT_SpacePointCollection::const_iterator sp_begin= colNext->begin();
      FaserSCT_SpacePointCollection::const_iterator sp_end= colNext->end();
      Identifier elementID = colNext->identify();
      for (; sp_begin != sp_end; ++sp_begin){
	const FaserSCT_SpacePoint* sp=&(**sp_begin);
	if(pow(sp->globalPosition().x()-x,2)+pow(sp->globalPosition().y()-y,2)+pow(sp->globalPosition().z()-z,2)<0.001)return sp;
      }
    }
    return nullptr;
  }

  /*
   * need to be updated
   int TrackerSPFit::findClusterIndex(const Tracker::FaserSCT_SpacePoint* sp, const Tracker::FaserSCT_ClusterCollection* ptr)const {
   if (ptr!=nullptr) {
//loop though collection to find matching PRD.
Tracker::FaserSCT_ClusterCollection::const_iterator collIt    = ptr->begin();
Tracker::FaserSCT_ClusterCollection::const_iterator collItEnd = ptr->end();
for ( ; collIt!=collItEnd; collIt++) {
if ( (*collIt)->identify()==id1 ) 
clus1= &(**collIt);
}
}

}
*/

}
