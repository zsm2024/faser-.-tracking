/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
   */

/**  @file TrackSeedFinder.h
 *   an algorithm for finding track seeds 
 *   @author Andrea Coccaro
 *   @date May 2021
 */


#ifndef TRACKERSEEDFINDER_TRACKERSEEDFINDER_H
#define TRACKERSEEDFINDER_TRACKERSEEDFINDER_H

#include "StoreGate/ReadCondHandleKey.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "Identifier/Identifier.h"

#include "GeneratorObjects/McEventCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerPrepRawData/TrackerClusterContainer.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointOverlapCollection.h"
#include "TrackerSpacePoint/SpacePointForSeedCollection.h"

#include "TrackerSpacePoint/TrackerSeedCollection.h"
#include "TrackerSpacePoint/TrackerSeed.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerSimData/TrackerSimDataCollection.h"


#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"

#include <string>

using namespace std;

class FaserSCT_ID;

namespace Tracker {

  class TrackerSeedFinder:public AthReentrantAlgorithm {

  public:

    TrackerSeedFinder(const std::string& name, ISvcLocator* pSvcLocator);
    
    virtual ~TrackerSeedFinder() = default;
    
    virtual StatusCode initialize() override;
    
    virtual StatusCode execute (const EventContext& ctx) const override;
    
    virtual StatusCode finalize() override;
    
  private:

    struct seed {
      vector<const FaserSCT_SpacePoint*> vsp;
      double axz, bxz, ayz, byz;
      double chi2_xz, chi2_yz;
      string station;
      int num;

      void add_sp(const FaserSCT_SpacePoint* sp) {
	vsp.push_back(sp);
      }

      void add_vsp(vector<const FaserSCT_SpacePoint*> v) {
	for (vector<const FaserSCT_SpacePoint*>::iterator it = v.begin(); it != v.end(); ++it) {	
	  vsp.push_back((*it));
	}
      }
    };

    StatusCode make_triplets(vector< vector<const FaserSCT_SpacePoint*> >&, vector<seed>&, string) const;

    TrackerSeedFinder() = delete;
    TrackerSeedFinder(const TrackerSeedFinder&) =delete;
    TrackerSeedFinder &operator=(const TrackerSeedFinder&) = delete;
    SG::ReadHandleKey<FaserSCT_SpacePointContainer>  m_Sct_spcontainerKey{this, "SpacePointsSCTName", "SCT spContainer"};
    
    SG::WriteHandleKey<SpacePointForSeedCollection>  m_seed_spcontainerKey{this, "FaserSpacePointsSeedsName", "SpacePointForSeedCollection", "SpacePointForSeedCollection"};
    SG::WriteHandleKey<Tracker::TrackerSeedCollection>  m_trackerSeedContainerKey{this, "FaserTrackerSeedName", "FaserTrackerSeedCollection", "FaserTrackerSeedCollection"};
    
    SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    
    const FaserSCT_ID* m_idHelper{nullptr};
    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfSPCollection{0};
    mutable std::atomic<int> m_numberOfEmptySPCollection{0};
    mutable std::atomic<int> m_numberOfSP{0};
    
    TH1* m_hist_n;
    TH1* m_hist_x;
    TH1* m_hist_y;
    TH1* m_hist_z;
    TH1* m_hist_r;
    TH1* m_hist_phi;
    TH1* m_hist_eta;
    TH1* m_hist_layer;
    TH1* m_hist_strip;
    TH1* m_hist_station;
    TH2* m_hist_x_z;
    TH2* m_hist_y_z;
    TH1* m_hist_sp_plane;
    TH1* m_hist_sp_layer;
    TH1* m_hist_sp_station;
    TH1* m_hist_dx;
    TH1* m_hist_dy;
    TH1* m_hist_dx_slope;
    TH1* m_hist_dy_slope;
    TH1* m_hist_dx_intercept;
    TH1* m_hist_dx_intercept_after;
    TH1* m_hist_line;
    TH1* m_hist_nseed;
    TH1* m_hist_nsp1;
    TH1* m_hist_nsp2;
    
    ServiceHandle<ITHistSvc>  m_thistSvc;
    
    mutable int N_1_0=0, N_1_1=0, N_1_2=0, N_2_0=0, N_2_1=0, N_2_2=0, N_3_0=0, N_3_1=0, N_3_2=0;
    
  };
  
}
#endif // TRACKERSEEDFINDER_TRACKERSEEDFINDER_H
