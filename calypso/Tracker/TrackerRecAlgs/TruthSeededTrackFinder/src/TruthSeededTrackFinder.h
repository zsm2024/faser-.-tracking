// -*- C++ -*-

/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

#ifndef TRUTHSEEDEDTRACKFINDER_TRUTHSEEDEDTRACKFINDER_H
#define TRUTHSEEDEDTRACKFINDER_TRUTHSEEDEDTRACKFINDER_H

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
#include "TrackerSpacePoint/SpacePointForSeedCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointOverlapCollection.h"
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

class FaserSCT_ID;

namespace Tracker
{

class TruthSeededTrackFinder:public AthReentrantAlgorithm {

  public:

    /**
     * @name AthReentrantAlgorithm methods
     */
    TruthSeededTrackFinder(const std::string& name,	ISvcLocator* pSvcLocator);

    virtual ~TruthSeededTrackFinder() = default;

    virtual StatusCode initialize() override;

    virtual StatusCode execute (const EventContext& ctx) const override;

    virtual StatusCode finalize() override;


  private:
    TruthSeededTrackFinder() = delete;
    TruthSeededTrackFinder(const TruthSeededTrackFinder&) =delete;
    TruthSeededTrackFinder &operator=(const TruthSeededTrackFinder&) = delete;
    SG::ReadHandleKey<FaserSCT_SpacePointContainer>  m_Sct_spcontainerKey{this, "SpacePointsSCTName", "SCT spContainer"};

    SG::WriteHandleKey<SpacePointForSeedCollection>  m_seed_spcontainerKey{this, "FaserSpacePointsSeedsName", "SpacePointForSeedCollection", "SpacePointForSeedCollection"};

    SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

    SG::ReadHandleKey<McEventCollection> m_mcEventKey       { this, "McEventCollection", "BeamTruthEvent" };
    SG::ReadHandleKey<FaserSiHitCollection> m_faserSiHitKey { this, "FaserSiHitCollection", "SCT_Hits" };
    SG::ReadHandleKey<FaserSCT_RDO_Container> m_faserRdoKey { this, "FaserSCT_RDO_Container", "SCT_RDOs"};
    SG::ReadHandleKey<TrackerSimDataCollection> m_sctMap {this, "TrackerSimDataCollection", "SCT_SDO_Map"};


    const FaserSCT_ID* m_idHelper{nullptr};
    mutable std::atomic<int> m_numberOfEvents{0};
    mutable std::atomic<int> m_numberOfSPCollection{0};
    mutable std::atomic<int> m_numberOfEmptySPCollection{0};
    mutable std::atomic<int> m_numberOfSP{0};
    mutable std::atomic<int> m_numberOfNoMap{0};
    mutable std::atomic<int> m_numberOfHits{0};
    mutable std::atomic<int> m_numberOfMatchSP{0};
    mutable std::atomic<int> m_numberOfFills{0}; 
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
    TH2* m_hist_x_y_plane0;
    TH2* m_hist_x_y_plane1;
    TH2* m_hist_x_y_plane2;
    TH2* m_hist_x_y_plane3;
    TH2* m_hist_x_y_plane4;
    TH2* m_hist_x_y_plane5;
    TH2* m_hist_x_y_plane6;
    TH2* m_hist_x_y_plane7;
    TH2* m_hist_x_y_plane8;
    TH2* m_hist_x_z;
    TH2* m_hist_y_z;
    TH1* m_hist_sp_sensor;
    TH1* m_hist_sp_module;
    TH1* m_hist_sp_row;
    TH1* m_hist_sp_plane;
    TH1* m_hist_sp_layer;
    TH1* m_hist_sp_station;
    //!!!!!!!!!!!!!!!!!!!!!
    TH1* m_hist_InitReso_px;
    TH1* m_hist_InitReso_py;
    TH1* m_hist_InitReso_pz;
    //!!!!!!!!!!!!!!!!!!!!!
    ServiceHandle<ITHistSvc>  m_thistSvc;

};

}
#endif // TRUTHSEEDEDTRACKFINDER_TRUTHSEEDEDTRACKFINDER_H
