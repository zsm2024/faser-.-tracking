// -*- C++ -*-

/*
   Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRACKERSPFIT_TRACKERSPACEPOINTMAKERALG_H
#define TRACKERSPFIT_TRACKERSPACEPOINTMAKERALG_H

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Identifier/Identifier.h"

#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointOverlapCollection.h"
#include "TrackerSpacePoint/TrackerSeedCollection.h"
#include "TrackerSpacePoint/TrackerSeed.h"
#include "TrkEventPrimitives/LocalParameters.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"
#include "TGraph2DErrors.h"
#include "TMath.h"
#include "Math/Functor.h"
#include "Fit/Fitter.h"
#include "Math/Vector3D.h"

#include <string>

class FaserSCT_ID;
namespace Tracker
{

  class TrackerSPFit:public AthReentrantAlgorithm {

    public:

      /**
       * @name AthReentrantAlgorithm methods
       */
      //@{
      TrackerSPFit(const std::string& name,
	  ISvcLocator* pSvcLocator);

      virtual ~TrackerSPFit() = default;

      virtual StatusCode initialize() override;

      virtual StatusCode execute (const EventContext& ctx) const override;

      virtual StatusCode finalize() override;

      struct SP_Seed{Amg::Vector3D pos; Amg::MatrixX cov; int layer; int module;};
      struct SP_TSOS{Amg::Vector3D pos; Amg::Vector3D predicted; Amg::Vector3D err; int layer;int module; double chi2=-1.; double edm=-1.; int ndf=-1;double p0=0.;double p1=0.;double p2=0.;double p3=0;};
      //struct SP_TSOS{Amg::Vector3D pos; Amg::Vector3D predicted; Amg::Vector3D err; int layer;int module;  const double *p;};
      std::vector<SP_TSOS> makeTrackSeg(std::vector<SP_Seed> sps, double maxchi2) const;
      std::vector<SP_TSOS> makeTrackSeg(SP_Seed sp0, SP_Seed sp1, SP_Seed sp2, double maxchi2) const;
      std::vector<SP_TSOS> makeTrackSeg(SP_Seed sp0, SP_Seed sp1, SP_Seed sp2) const;
      Amg::Vector3D predicted(double z, const double *p) const;

      // function Object to be minimized
      struct SumDistance2 {
	// the TGraph is a data member of the object
	TGraph2DErrors *fGraph;
	SumDistance2(TGraph2DErrors *g) : fGraph(g) {}
	// calculate distance line-point
	double distance2(double x,double y,double z, double xe, double ye, const double *p) {
	  double xpred=p[0]+p[1]*z;
	  double ypred=p[2]+p[3]*z;
	  double xchi2= (xpred-x)*(xpred-x)/xe/xe;
	  double ychi2= (ypred-y)*(ypred-y)/ye/ye;
	  double d2=xchi2+ychi2;
	  return d2;
	}
	// implementation of the function to be minimized
	double operator() (const double *par) {
	  assert(fGraph != 0);
	  double * x = fGraph->GetX();
	  double * y = fGraph->GetY();
	  double * z = fGraph->GetZ();
	  double * xe = fGraph->GetEX();
	  double * ye = fGraph->GetEY();
	  int npoints = fGraph->GetN();
	  double sum = 0;
	  for (int i  = 0; i < npoints; ++i) {
	    double d = distance2(x[i],y[i],z[i],xe[i],ye[i],par);
	    sum += d;
	  }
	  return sum;
	}
      };

    private:
      void initializeTree();
      void clearVariables() const;
      mutable TTree* m_tree;
      TrackerSPFit() = delete;
      TrackerSPFit(const TrackerSPFit&) =delete;
      TrackerSPFit &operator=(const TrackerSPFit&) = delete;

      SG::ReadHandleKey<FaserSCT_SpacePointContainer>  m_Sct_spcontainerKey{this, "SpacePointsSCTName", "SCT_SpacePointContainer"};
      SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer>  m_Sct_clcontainerKey{this, "SCT_ClustersName", "SCT clContainer"}; 

      SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
      SG::WriteHandleKey<Tracker::TrackerSeedCollection>  m_trackerSeedContainerKey{this, "FaserTrackerSeedName", "FaserTrackerSeedCollection" };
      const Tracker::FaserSCT_SpacePoint* findSpacePoint(const FaserSCT_SpacePointContainer* spcontainer, double x, double y, double z) const;

      const FaserSCT_ID* m_idHelper{nullptr};
      mutable std::atomic<int> m_numberOfEvents{0};
      mutable std::atomic<int> m_numberOfSCT{0};
      mutable std::atomic<int> m_numberOfTrack{0};
      mutable std::atomic<int> m_numberOfSP{0};
      mutable std::atomic<int> m_numberOfCluster{0};
      mutable std::atomic<int> m_numberOfSPCol{0};
      mutable std::atomic<int> m_numberOfClusterCol{0};
      //@}
      TH1* m_hist_x;
      TH1* m_hist_y;
      TH1* m_hist_z;
      TH1* m_hist_r;
      TH1* m_hist_eta;
      TH1* m_hist_phi;
      TH1* m_hist_station;
      TH1* m_hist_strip;
      TH1* m_hist_layer;
      TH2* m_hist_x_y_plane0;
      TH2* m_hist_x_y_plane1;
      TH2* m_hist_x_y_plane2;
      TH2* m_hist_x_y_plane3;
      TH2* m_hist_x_y_plane4;
      TH2* m_hist_x_y_plane5;
      TH2* m_hist_x_y_plane6;
      TH1* m_hist_x_y_plane7;
      TH1* m_hist_x_y_plane8;
      TH1* m_residual_y_plane0;
      TH1* m_residual_y_plane1;
      TH1* m_residual_y_plane2;
      TH1* m_residual_y_plane3;
      TH1* m_residual_y_plane4;
      TH1* m_residual_y_plane5;
      TH1* m_residual_y_plane6;
      TH1* m_residual_y_plane7;
      TH1* m_residual_y_plane8;
      TH1* m_residual_x_plane0;
      TH1* m_residual_x_plane1;
      TH1* m_residual_x_plane2;
      TH1* m_residual_x_plane3;
      TH1* m_residual_x_plane4;
      TH1* m_residual_x_plane5;
      TH1* m_residual_x_plane6;
      TH1* m_residual_x_plane7;
      TH1* m_residual_x_plane8;
      TH1* m_chi2;
      TH1* m_edm;
      TH1* m_ndf;
      ServiceHandle<ITHistSvc>  m_thistSvc;
      //for alignment constants
      TProfile* m_preYhistX_s1l1m0;
      TProfile* m_preYhistY_s1l1m0;
      TProfile* m_preXhistX_s1l1m0;
      TProfile* m_preXhistY_s1l1m0;
      TProfile* m_preYhistX_s1l1m1;
      TProfile* m_preYhistY_s1l1m1;
      TProfile* m_preXhistX_s1l1m1;
      TProfile* m_preXhistY_s1l1m1;
      TProfile* m_preYhistX_s1l1m2;
      TProfile* m_preYhistY_s1l1m2;
      TProfile* m_preXhistX_s1l1m2;
      TProfile* m_preXhistY_s1l1m2;
      TProfile* m_preYhistX_s1l1m3;
      TProfile* m_preYhistY_s1l1m3;
      TProfile* m_preXhistX_s1l1m3;
      TProfile* m_preXhistY_s1l1m3;
      TProfile* m_preYhistX_s1l1m4;
      TProfile* m_preYhistY_s1l1m4;
      TProfile* m_preXhistX_s1l1m4;
      TProfile* m_preXhistY_s1l1m4;
      TProfile* m_preYhistX_s1l1m5;
      TProfile* m_preYhistY_s1l1m5;
      TProfile* m_preXhistX_s1l1m5;
      TProfile* m_preXhistY_s1l1m5;
      TProfile* m_preYhistX_s1l1m6;
      TProfile* m_preYhistY_s1l1m6;
      TProfile* m_preXhistX_s1l1m6;
      TProfile* m_preXhistY_s1l1m6;
      TProfile* m_preYhistX_s1l1m7;
      TProfile* m_preYhistY_s1l1m7;
      TProfile* m_preXhistX_s1l1m7;
      TProfile* m_preXhistY_s1l1m7;

      mutable int m_eventNumber;
      mutable std::vector<double> m_sp_track_x;
      mutable std::vector<double> m_sp_track_y;
      mutable std::vector<double> m_sp_track_z;
      mutable std::vector<double> m_sp_track_x_err;
      mutable std::vector<double> m_sp_track_y_err;
      mutable std::vector<double> m_sp_track_z_err;
      mutable std::vector<double> m_sp_track_x_predicted;
      mutable std::vector<double> m_sp_track_y_predicted;
      mutable std::vector<double> m_sp_track_z_predicted;
      mutable std::vector<double> m_sp_track_x_residual;
      mutable std::vector<double> m_sp_track_y_residual;
      mutable std::vector<double> m_sp_track_z_residual;
      mutable std::vector<int> m_sp_track_station;
      mutable std::vector<int> m_sp_track_layer;
      mutable std::vector<int> m_sp_track_module;
      mutable std::vector<int> m_trackId;
      mutable std::vector<double> m_track_chi2;
      mutable std::vector<double> m_track_edm;
      mutable std::vector<int> m_track_ndf;
      mutable std::vector<double> m_track_p0;
      mutable std::vector<double> m_track_p1;
      mutable std::vector<double> m_track_p2;
      mutable std::vector<double> m_track_p3;
      double m_maxchi2;
      bool m_bias;
      bool m_saveallcluster;
      mutable std::vector<double> m_cluster_all_global_x;
      mutable std::vector<double> m_cluster_all_global_y;
      mutable std::vector<double> m_cluster_all_global_z;
      mutable std::vector<double> m_cluster_all_local_x;
      mutable std::vector<double> m_cluster_all_local_y;
      mutable std::vector<double> m_cluster_all_phiwidth;
      mutable std::vector<long long int> m_cluster_all_identify;
      mutable std::vector<int> m_cluster_all_size;
      mutable std::vector<int> m_cluster_all_station;
      mutable std::vector<int> m_cluster_all_layer;
      mutable std::vector<int> m_cluster_all_module;
      mutable std::vector<int> m_cluster_all_side;
      mutable std::vector<int> m_cluster_all_strip;
      bool m_saveallsp;
      mutable std::vector<double> m_sp_all_x_local;
      mutable std::vector<double> m_sp_all_y_local;
      mutable std::vector<double> m_sp_all_x;
      mutable std::vector<double> m_sp_all_y;
      mutable std::vector<double> m_sp_all_z;
      mutable std::vector<int> m_sp_all_station;
      mutable std::vector<int> m_sp_all_layer;
      mutable std::vector<int> m_sp_all_module;
      mutable std::vector<long long int> m_sp_all_identify0;
      mutable std::vector<long long int> m_sp_all_identify1;
      bool m_doublets;

  };

}
#endif // TrackerSpacePointMakerAlg_TRACKERSPACEPOINTMAKERALG_H
