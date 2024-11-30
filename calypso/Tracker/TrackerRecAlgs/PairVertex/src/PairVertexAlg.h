/*
Copyright (C) 2022 CERN for the benefit of the FASER collaboration
*/


#pragma once

#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"

#include "FaserActsKalmanFilter/IFiducialParticleTool.h"
#include "FaserActsKalmanFilter/ITrackTruthMatchingTool.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthEventContainer.h"

#include <vector>
#include "TTree.h"
#include <TH1.h>
#include <TProfile.h>
#include "AthenaBaseComps/AthHistogramming.h"
#include "StoreGate/ReadHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"


class FaserSCT_ID;
class VetoNuID;
class TTree;
class VetoID;
class TriggerID;
class PreshowerID;
class EcalID;

namespace  TrackerDD
{
    class SCT_DetectorManager;
}

namespace Tracker
{
    class PairVertexAlg : public AthReentrantAlgorithm,AthHistogramming  
    {
    public:
        PairVertexAlg(const std::string& name, ISvcLocator* pSvcLocator);

        virtual StatusCode initialize() override;
        virtual StatusCode execute(const EventContext& ctx) const override;
        virtual StatusCode finalize() override;
        const ServiceHandle <ITHistSvc> &histSvc() const;
        
    private:

        void clearTree() const;
        void addBranch(const std::string &name,float* var);
        void addBranch(const std::string &name,unsigned int* var);
        void addWaveBranches(const std::string &name, int nchannels, int first);
        void FillWaveBranches(const xAOD::WaveformHitContainer &wave) const;
        bool waveformHitOK(const xAOD::WaveformHit* hit) const;

        ServiceHandle<ITHistSvc> m_histSvc;

        double m_MeV2GeV = 1e-3;
        const FaserSCT_ID* m_idHelper {nullptr};
        const VetoNuID*    m_vetoNuHelper;
        const VetoID*      m_vetoHelper;
        const TriggerID*   m_triggerHelper;
        const PreshowerID* m_preshowerHelper;
        const EcalID*      m_ecalHelper;
        
        const TrackerDD::SCT_DetectorManager* m_detMgr {nullptr};
        SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey { this, "McEventCollection", "TruthEvent" };
        SG::ReadHandleKey<TrackCollection> m_trackCollectionKey { this, "TrackCollection", "CKFTrackCollection" };
        SG::ReadHandleKey<xAOD::WaveformHitContainer> m_vetoNuContainer { this, "VetoNuContainer", "VetoNuWaveformHits", "VetoNu hit container name" };
        SG::ReadHandleKey<xAOD::WaveformHitContainer> m_vetoContainer { this, "VetoContainer", "VetoWaveformHits", "Veto hit container name" };
        SG::ReadHandleKey<xAOD::WaveformHitContainer> m_triggerContainer { this, "TriggerContainer", "TriggerWaveformHits", "Trigger hit container name" };
        SG::ReadHandleKey<xAOD::WaveformHitContainer> m_preshowerContainer { this, "PreshowerContainer", "PreshowerWaveformHits", "Preshower hit container name" };
        SG::ReadHandleKey<xAOD::WaveformHitContainer> m_ecalContainer { this, "EcalContainer", "CaloWaveformHits", "Ecal hit container name" };

        ToolHandle<IFiducialParticleTool> m_fiducialParticleTool {this, "FiducialParticleTool", "FiducialParticleTool"};
        ToolHandle<ITrackTruthMatchingTool> m_trackTruthMatchingTool {this, "TrackTruthMatchingTool", "TrackTruthMatchingTool"}; 
        SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainer { this, "ParticleContainer", "TruthParticles", "Truth particle container name." };

        mutable std::atomic<size_t> m_numberOfEvents{0};
        mutable std::atomic<size_t> m_numberOfPositiveTracks{0};
        mutable std::atomic<size_t> m_numberOfNegativeTracks{0};
        mutable std::atomic<size_t> m_numberOfOppositeSignPairs{0};
        mutable std::atomic<size_t> m_numberOfNoParameterTracks{0};
        mutable std::atomic<size_t> m_invalidTrackContainerEntries{0};
        mutable std::atomic<size_t> m_numberOfNullParameters{0};
        mutable std::atomic<size_t> m_numberOfParametersWithoutSurface{0};
        mutable std::atomic<size_t> m_numberOfNoUpstreamTracks{0};
        
        mutable TTree* m_tree;

        //TRACK COLLECTION
        mutable int m_trackCount=0;
        
        mutable std::vector<double> m_tracksX;
        mutable std::vector<double> m_tracksY;
        mutable std::vector<double> m_tracksZ;
        mutable std::vector<double> m_tracksPx;
        mutable std::vector<double> m_tracksPy;
        mutable std::vector<double> m_tracksPz;
        mutable std::vector<double> m_tracksPmag;

        mutable std::vector<double> m_tracksCharge;
        mutable std::vector<double> m_sigmaQoverP;
        mutable std::vector<double> m_tracksChi2;
        mutable std::vector<double> m_tracksDoF;

        mutable std::vector<double> m_tracksTruthBarcode;
        mutable std::vector<double> m_tracksTruthPdg;
        mutable std::vector<double> m_tracksTruthCharge;
        mutable std::vector<double> m_tracksTruthPmag;
        mutable std::vector<bool>   m_tracksIsFiducial;

        mutable std::vector<int>    m_tracksnLayersHit;
        mutable std::vector<int>    m_tracksLayersIFT;
        mutable std::vector<int>    m_tracksIFThit;

        mutable std::vector<int>    m_trackPositiveIndicies;
        mutable std::vector<int>    m_trackNegativeIndicies;
        mutable int    m_trackD2i; //which track is daughter; -1 if  not matched
        mutable int    m_trackD3i;

        mutable std::vector<std::vector<int>> m_layerMap={{},{},{},{}};

        //WAVEFORM
        mutable float m_wave_localtime[15];
        mutable float m_wave_peak[15];
        mutable float m_wave_width[15];
        mutable float m_wave_integral[15];
        mutable float m_wave_raw_peak[15];
        mutable float m_wave_raw_integral[15];
        mutable float m_wave_baseline_mean[15];
        mutable float m_wave_baseline_rms[15];
        mutable unsigned int m_wave_status[15];

        //TRUTH
        DoubleProperty  m_minMomentum       { this, "MinMomentum", 50000.0, "Write out all truth particles with a momentum larger MinMomentum"};

        mutable std::vector<double> m_vertexPos = {-1,-1,-1};
        mutable std::vector<bool> m_isFiducial; // track is fiducial if there are simulated hits for stations 1 - 3 and the distance from the center is smaller than 100 mm

        mutable std::vector<int> m_truthParticleBarcode; // vector of barcodes of all truth particles with a momentum larger 50 GeV
        mutable std::vector<int> m_truthParticleMatchedTracks; // vector of number of tracks to which a truth particle is matched to
        mutable std::vector<bool> m_truthParticleIsFiducial; // vector of boolean showing whether a truth particle is fiducial

        mutable std::vector<int>    m_truthBarcode;
        mutable std::vector<int>    m_truthPdg;
        mutable std::vector<double> m_truthPmag;
        mutable std::vector<double> m_truthCharge;

        mutable std::vector<double> m_truthM_P;

        mutable std::vector<double> m_truthM_px;
        mutable std::vector<double> m_truthM_py;
        mutable std::vector<double> m_truthM_pz;

        mutable std::vector<double> m_truthM_x;
        mutable std::vector<double> m_truthM_y;
        mutable std::vector<double> m_truthM_z;


        mutable std::vector<double> m_truthd0_P;

        mutable std::vector<double> m_truthd0_px;
        mutable std::vector<double> m_truthd0_py;
        mutable std::vector<double> m_truthd0_pz;

        mutable std::vector<double> m_truthd0_x;
        mutable std::vector<double> m_truthd0_y;
        mutable std::vector<double> m_truthd0_z;

        mutable std::vector<double> m_truthd0_IsFiducial;
        mutable std::vector<double> m_truthd0_charge;

        mutable std::vector<double> m_truthd1_P;

        mutable std::vector<double> m_truthd1_px;
        mutable std::vector<double> m_truthd1_py;
        mutable std::vector<double> m_truthd1_pz;

        mutable std::vector<double> m_truthd1_x;
        mutable std::vector<double> m_truthd1_y;
        mutable std::vector<double> m_truthd1_z;

        mutable std::vector<double> m_truthd1_IsFiducial;
        mutable std::vector<double> m_truthd1_charge;


    };
    
    inline const ServiceHandle<ITHistSvc>& PairVertexAlg::histSvc() const {return m_histSvc;}

    }


