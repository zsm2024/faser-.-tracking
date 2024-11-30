// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef StatisticsAlg_TRACKERSPACEPOINTMAKERALG_H
#define StatisticsAlg_TRACKERSPACEPOINTMAKERALG_H

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

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include <string>
#include <vector>
#include "TH1.h"
#include "TH2.h"

#include <string>

class FaserSCT_ID;
namespace Tracker
{

  class StatisticsAlg:public AthReentrantAlgorithm {

  public:
  
    /**
     * @name AthReentrantAlgorithm methods
     */
    //@{
    StatisticsAlg(const std::string& name,
			      ISvcLocator* pSvcLocator);
    
    virtual ~StatisticsAlg() = default;
    
    virtual StatusCode initialize() override;
    
    virtual StatusCode execute (const EventContext& ctx) const override;

    virtual StatusCode finalize() override;


  private:
    StatisticsAlg() = delete;
    StatisticsAlg(const StatisticsAlg&) =delete;
    StatisticsAlg &operator=(const StatisticsAlg&) = delete;
    //@}

    SG::ReadHandleKey<FaserSCT_SpacePointContainer>  m_Sct_spcontainerKey{this, "SpacePointsSCTName", "SCT spContainer"};
    //@}
      SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer>  m_Sct_clcontainerKey{this, "SCT_ClustersName", "SCT clContainer"}; 

    SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
    //@}

    const FaserSCT_ID* m_idHelper{nullptr};
    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfSCT{0};
    mutable std::atomic<int> m_numberOfSP{0};
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
   TH2* m_hist_x_y_plane7;
   TH2* m_hist_x_y_plane8;
   ServiceHandle<ITHistSvc>  m_thistSvc;


  };

}
#endif // TrackerSpacePointMakerAlg_TRACKERSPACEPOINTMAKERALG_H
