#ifndef NTUPLEDUMPER_NTUPLEDUMPERALG_H
#define NTUPLEDUMPER_NTUPLEDUMPERALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODFaserLHC/FaserLHCData.h"
#include "xAODFaserTrigger/FaserTriggerData.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODFaserWaveform/WaveformClock.h"
#include "xAODFaserCalorimeter/CalorimeterHitContainer.h"
#include "xAODFaserCalorimeter/CalorimeterHit.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/IFiducialParticleTool.h"
#include "FaserActsKalmanFilter/ITrackTruthMatchingTool.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "xAODEventInfo/EventInfo.h"
//#include "StoreGate/ReadDecorHandle.h"
//#include "FaserActsVertexing/IVertexingTool.h"
#include "GeneratorObjects/McEventCollection.h"
#include <boost/dynamic_bitset.hpp>

#include "WaveformConditionsTools/IWaveformCableMappingTool.h"

#include <vector>
#include <nlohmann/json.hpp>

using HitSet = boost::dynamic_bitset<>;

class TTree;
class TH1;
class FaserSCT_ID;
class VetoNuID;
class VetoID;
class TriggerID;
class PreshowerID;
class EcalID;
namespace  TrackerDD
{
    class SCT_DetectorManager;
}

class NtupleDumperAlg : public AthReentrantAlgorithm, AthHistogramming {
public:
  NtupleDumperAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~NtupleDumperAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;
  const ServiceHandle <ITHistSvc> &histSvc() const;

private:

  mutable bool m_first;
  
  bool waveformHitOK(const xAOD::WaveformHit* hit) const;
  void clearTree() const;
  void clearTrackTruth() const;
  void addBranch(const std::string &name,float* var) const;
  void addBranch(const std::string &name,unsigned int* var) const;
  void defineWaveBranches() const;
  void addWaveBranches(const std::string &name, int nchannels, int first) const;
  void addWaveBranches(const std::string &name, std::list<int> channel_list) const;
  void FillWaveBranches(const xAOD::WaveformHitContainer &wave, bool isMC) const;
  void addCalibratedBranches(const std::string &name, int nchannels, int first);
  double radius(const Acts::Vector3 &position) const;

  ServiceHandle <ITHistSvc> m_histSvc;

  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventContainer { this, "EventContainer", "TruthEvents", "Truth event container name." };
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainer { this, "ParticleContainer", "TruthParticles", "Truth particle container name." };
  SG::ReadHandleKey<McEventCollection> m_mcEventContainer {this, "McEventCollection", "BeamTruthEvent", "MC event collection name"};
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollection {this, "TrackerSimDataCollection", "SCT_SDO_Map"};

  SG::ReadHandleKey<xAOD::FaserLHCData> m_lhcData {this, "FaserLHCDataKey", "FaserLHCData"};

  SG::ReadHandleKey<TrackCollection> m_trackCollection { this, "TrackCollection", "CKFTrackCollection", "Input track collection name" };
  SG::ReadHandleKey<TrackCollection> m_trackCollectionWithoutIFT { this, "TrackCollectionWithoutIFT", "CKFTrackCollectionWithoutIFT", "Input track collection name (without IFT)" };
  SG::ReadHandleKey<TrackCollection> m_trackSegmentCollection {this, "TrackSegmentCollection", "SegmentFit", "Input track segment collection name"};

  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_vetoNuContainer { this, "VetoNuContainer", "VetoNuWaveformHits", "VetoNu hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_vetoContainer { this, "VetoContainer", "VetoWaveformHits", "Veto hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_triggerContainer { this, "TriggerContainer", "TriggerWaveformHits", "Trigger hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_preshowerContainer { this, "PreshowerContainer", "PreshowerWaveformHits", "Preshower hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_ecalContainer { this, "EcalContainer", "CaloWaveformHits", "Ecal hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_ecal2Container { this, "Ecal2Container", "Calo2WaveformHits", "Ecal hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_caloNuContainer { this, "CaloNuContainer", "CaloNuWaveformHits", "CaloNu hit container name" };

  SG::ReadHandleKey<xAOD::CalorimeterHitContainer> m_ecalCalibratedContainer { this, "EcalCalibratedContainer", "CaloHits", "Ecal Calibrated hit container name" };
  SG::ReadHandleKey<xAOD::CalorimeterHitContainer> m_ecal2CalibratedContainer { this, "Ecal2CalibratedContainer", "Calo2Hits", "Ecal Calibrated hit container name" };
  SG::ReadHandleKey<xAOD::CalorimeterHitContainer> m_calonuCalibratedContainer { this, "CaloNuCalibratedContainer", "CaloNuHits", "CaloNu Calibrated hit container name" };
  SG::ReadHandleKey<xAOD::CalorimeterHitContainer> m_preshowerCalibratedContainer { this, "preshowerCalibratedContainer", "PreshowerHits", "Preshower Calibrated hit container name" };

  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_clusterContainer { this, "ClusterContainer", "SCT_ClusterContainer", "Tracker cluster container name" };
  SG::ReadHandleKey<FaserSCT_SpacePointContainer> m_spacePointContainerKey { this, "SpacePoints", "SCT_SpacePointContainer", "space point container"};
  SG::ReadHandleKey<FaserSiHitCollection> m_siHitCollectionKey{this, "FaserSiHitCollection", "SCT_Hits"};

  SG::ReadHandleKey<xAOD::FaserTriggerData> m_FaserTriggerData     { this, "FaserTriggerDataKey", "FaserTriggerData", "ReadHandleKey for xAOD::FaserTriggerData"};
  SG::ReadHandleKey<xAOD::WaveformClock> m_ClockWaveformContainer     { this, "WaveformClockKey", "WaveformClock", "ReadHandleKey for ClockWaveforms Container"};
  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "ReadHandleKey for EventInfo"};
  ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool { this, "ExtrapolationTool", "FaserActsExtrapolationTool" };
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  ToolHandle<ITrackTruthMatchingTool> m_trackTruthMatchingTool {this, "TrackTruthMatchingTool", "TrackTruthMatchingTool"};
  ToolHandle<IFiducialParticleTool> m_fiducialParticleTool {this, "FiducialParticleTool", "FiducialParticleTool"};
  //ToolHandle<FaserTracking::IVertexingTool> m_vertexingTool { this, "VertexingTool", "FaserTracking::PointOfClosestApproachSearchTool"};
  ToolHandle<IWaveformCableMappingTool> m_mappingTool { this, "WaveformCableMappingTool", "WaveformCableMappingTool" };

  const TrackerDD::SCT_DetectorManager* m_detMgr {nullptr};

  const FaserSCT_ID* m_sctHelper;

  BooleanProperty m_useIFT               { this, "UseIFT", false, "Use IFT tracks" };
  BooleanProperty m_doCalib              { this, "DoCalib", true, "Fill calibrated calorimeter quantities" };
  BooleanProperty m_doBlinding           { this, "DoBlinding", true, "Blinding will not output events with no veto signal adn Calo signal > 20 GeV e-" };
  BooleanProperty m_onlyBlinded          { this, "OnlyBlinded", false, "Only events that would be blinded are saved" };
  BooleanProperty m_doTrigFilter         { this, "DoTrigFilter", false, "Only events that pass trigger cuts are passed" };
  BooleanProperty m_doScintFilter        { this, "DoScintFilter", false, "Only events that pass scintillator coincidence cuts are passed" };
  BooleanProperty m_doTrackFilter        { this, "DoTrackFilter", true, "Only events that have >= 1 long track are passed, also non-colliding events with a track or calo signal are passed (default)" };
  BooleanProperty m_stableOnly           { this, "StableOnly", true, "Only events recorded during stable beams are saved (default)" };

  BooleanProperty m_useFlukaWeights      { this, "UseFlukaWeights", false, "Flag to weight events according to value stored in HepMC::GenEvent" };
  BooleanProperty m_useGenieWeights      { this, "UseGenieWeights", false, "Flag to weight events according to Genie luminosity" };
  IntegerProperty m_flukaCollisions      { this, "FlukaCollisions", 137130000, "Number of proton-proton collisions in FLUKA sample." };
  DoubleProperty  m_flukaCrossSection    { this, "FlukaCrossSection", 80.0, "Fluka p-p inelastic cross-section in millibarns." };
  DoubleProperty  m_genieLuminosity      { this, "GenieLuminosity", 150.0, "Genie luminosity in inverse fb." };
  DoubleProperty  m_minMomentum          { this, "MinMomentum", 50000.0, "Write out all truth particles with a momentum larger MinMomentum"};

  DoubleProperty m_blindingCaloThreshold {this, "BlindingCaloThreshold", 100000.0, "Blind events with Ecal energy above threshold (in MeV)"}; 
  DoubleProperty m_caloMC_FudgeFactor    {this, "CaloFudgeFactorMC", 1.088, "Correct the MC energy calibration by this fudge factor"};

  BooleanProperty m_applyGoodRunsList    {this, "ApplyGoodRunsList", false, "Only write out events passing GRL (data only)"};
  StringProperty m_goodRunsList          {this, "GoodRunsList", "", "GoodRunsList in json format"};

  // track quality cuts
  UnsignedIntegerProperty m_minLayers{this, "minLayers", 7, "Miminimum number of layers of a track."};
  UnsignedIntegerProperty m_minHits{this, "minHits", 12, "Miminimum number of hits of a track."};
  DoubleProperty m_maxChi2NDOF{this, "maxChi2NDOF", 25, "Maximum chi2 per degree of freedom."};
  BooleanProperty m_runVertexing{ this, "RunVertexing", true, "Run the vertexing, defaults to true." };

  // json object to hold data read from GRL file (or empty if not)
  nlohmann::json m_grl;

  double m_baseEventCrossSection {1.0};
  const double kfemtoBarnsPerMilliBarn {1.0e12};

  mutable TTree* m_tree;

  //mutable unsigned int n_wave_chan;  // Actual number of waveform channels
  const static unsigned int max_chan=32;
  mutable TH1* m_HistRandomCharge[max_chan];

  mutable unsigned int m_run_number;
  mutable unsigned int m_event_number;
  mutable unsigned int m_event_time;
  mutable unsigned int m_event_timeNSOffset;
  mutable unsigned int m_bcid;
  mutable unsigned int m_in_grl;

  mutable unsigned int m_fillNumber;
  mutable float m_betaStar;
  mutable float m_crossingAngle;
  mutable int m_distanceToCollidingBCID;
  mutable int m_distanceToUnpairedB1;
  mutable int m_distanceToUnpairedB2;
  mutable int m_distanceToInboundB1;
  mutable unsigned int m_distanceToTrainStart;
  mutable unsigned int m_distanceToPreviousColliding;

  mutable unsigned int m_tbp;
  mutable unsigned int m_tap;
  mutable unsigned int m_inputBits;
  mutable unsigned int m_inputBitsNext;

  mutable float m_wave_localtime[max_chan];
  mutable float m_wave_triggertime[max_chan];
  mutable float m_wave_bcidtime[max_chan];
  mutable float m_wave_peak[max_chan];
  mutable float m_wave_width[max_chan];
  mutable float m_wave_charge[max_chan];

  mutable float m_wave_raw_peak[max_chan];
  mutable float m_wave_raw_charge[max_chan];
  mutable float m_wave_baseline_mean[max_chan];
  mutable float m_wave_baseline_rms[max_chan];
  mutable unsigned int m_wave_status[max_chan];
 
  mutable unsigned int m_scintHit;
 
  mutable float m_calibrated_nMIP[max_chan];
  mutable float m_calibrated_E_dep[max_chan];
  mutable float m_calibrated_E_EM[max_chan];

  mutable float m_calo_total_nMIP;
  mutable float m_calo_total_E_dep;
  mutable float m_calo_total_fit_E_EM;
  mutable float m_calo_total_raw_E_EM;
  mutable float m_calo_total_E_EM;

  mutable float m_calo_total_fit_E_EM_fudged;
  mutable float m_calo_total_raw_E_EM_fudged;
  mutable float m_calo_total_E_EM_fudged;

  mutable float m_preshower_total_nMIP;
  mutable float m_preshower_total_E_dep;

  mutable float m_Calo0_Edep;
  mutable float m_Calo1_Edep;
  mutable float m_Calo2_Edep;
  mutable float m_Calo3_Edep;

  mutable float m_Calo_Total_Edep;
  mutable float m_Preshower12_Edep;
  mutable float m_Preshower13_Edep;

  mutable float m_clock_phase;

  mutable unsigned int m_station0Clusters;
  mutable unsigned int m_station1Clusters;
  mutable unsigned int m_station2Clusters;
  mutable unsigned int m_station3Clusters;

  mutable unsigned int m_nspacepoints;
  mutable std::vector<double> m_spacepointX;
  mutable std::vector<double> m_spacepointY;
  mutable std::vector<double> m_spacepointZ;

  mutable unsigned int m_ntracksegs;
  mutable std::vector<double> m_trackseg_Chi2;
  mutable std::vector<double> m_trackseg_DoF;
  mutable std::vector<double> m_trackseg_x;
  mutable std::vector<double> m_trackseg_y;
  mutable std::vector<double> m_trackseg_z;
  mutable std::vector<double> m_trackseg_px;
  mutable std::vector<double> m_trackseg_py;
  mutable std::vector<double> m_trackseg_pz;

  mutable int    m_longTracks;
  mutable int m_propagationError;
  mutable std::vector<int> m_module_eta0;
  mutable std::vector<int> m_module_phi0;
  mutable std::vector<unsigned long> m_hitSet;
  mutable std::vector<double> m_Chi2;
  mutable std::vector<double> m_DoF;
  mutable std::vector<double> m_xup;
  mutable std::vector<double> m_yup;
  mutable std::vector<double> m_zup;
  mutable std::vector<double> m_pxup;
  mutable std::vector<double> m_pyup;
  mutable std::vector<double> m_pzup;
  mutable std::vector<double> m_pup;
  mutable std::vector<double> m_xdown;
  mutable std::vector<double> m_ydown;
  mutable std::vector<double> m_zdown;
  mutable std::vector<double> m_pxdown;
  mutable std::vector<double> m_pydown;
  mutable std::vector<double> m_pzdown;
  mutable std::vector<double> m_pdown;
  mutable std::vector<int> m_charge;
  mutable std::vector<unsigned int> m_nLayers;
  mutable std::vector<unsigned int> m_nHit0;
  mutable std::vector<unsigned int> m_nHit1;
  mutable std::vector<unsigned int> m_nHit2;
  mutable std::vector<unsigned int> m_nHit3;
  mutable std::vector<double> m_xVetoNu;
  mutable std::vector<double> m_yVetoNu;
  mutable std::vector<double> m_thetaxVetoNu;
  mutable std::vector<double> m_thetayVetoNu;
  mutable std::vector<double> m_xVetoStation1;
  mutable std::vector<double> m_yVetoStation1;
  mutable std::vector<double> m_thetaxVetoStation1;
  mutable std::vector<double> m_thetayVetoStation1;
  mutable std::vector<double> m_xVetoStation2;
  mutable std::vector<double> m_yVetoStation2;
  mutable std::vector<double> m_thetaxVetoStation2;
  mutable std::vector<double> m_thetayVetoStation2;
  mutable std::vector<double> m_xTrig;
  mutable std::vector<double> m_yTrig;
  mutable std::vector<double> m_thetaxTrig;
  mutable std::vector<double> m_thetayTrig;
  mutable std::vector<double> m_xPreshower1;
  mutable std::vector<double> m_yPreshower1;
  mutable std::vector<double> m_thetaxPreshower1;
  mutable std::vector<double> m_thetayPreshower1;
  mutable std::vector<double> m_xPreshower2;
  mutable std::vector<double> m_yPreshower2;
  mutable std::vector<double> m_thetaxPreshower2;
  mutable std::vector<double> m_thetayPreshower2;
  mutable std::vector<double> m_xCalo;
  mutable std::vector<double> m_yCalo;
  mutable std::vector<double> m_thetaxCalo;
  mutable std::vector<double> m_thetayCalo;
  mutable std::vector<double> m_x_atMaxRadius;
  mutable std::vector<double> m_y_atMaxRadius;
  mutable std::vector<double> m_z_atMaxRadius;
  mutable std::vector<double> m_r_atMaxRadius;

  mutable std::vector<int> m_t_pdg; // pdg code of the truth matched particle
  mutable std::vector<int> m_t_barcode; // barcode of the truth matched particle
  mutable std::vector<int> m_t_pdg_parent; // pdg code of the parent of the truth matched particle
  mutable std::vector<int> m_t_barcode_parent; // barcode of the parent of the truth matched particle
  mutable std::vector<double> m_t_truthHitRatio; // ratio of hits on track matched to the truth particle over all hits on track
  mutable std::vector<double> m_t_prodVtx_x; // x component of the production vertex in mm
  mutable std::vector<double> m_t_prodVtx_y; // y component of the production vertex in mm
  mutable std::vector<double> m_t_prodVtx_z; // z component of the production vertex in mm
  mutable std::vector<double> m_t_decayVtx_x; // x component of the decay vertex in mm
  mutable std::vector<double> m_t_decayVtx_y; // y component of the decay vertex in mm
  mutable std::vector<double> m_t_decayVtx_z; // z component of the decay vertex in mm
  mutable std::vector<double> m_t_px; // truth momentum px in MeV
  mutable std::vector<double> m_t_py;  // truth momentum py in MeV
  mutable std::vector<double> m_t_pz;  // truth momentum pz in MeV
  mutable std::vector<double> m_t_theta; // angle of truth particle with respsect to the beam axis in rad, theta = arctan(sqrt(px * px + py * py) / pz)
  mutable std::vector<double> m_t_phi; // polar angle of truth particle in rad, phi = arctan(py / px)
  mutable std::vector<double> m_t_p; // truth momentum p in MeV
  mutable std::vector<double> m_t_pT; // transverse truth momentum pT in MeV
  mutable std::vector<double> m_t_eta; // eta of truth particle
  mutable std::array<std::vector<double>, 4> m_t_st_x; // vector of the x components of the simulated hits of the truth particle for each station
  mutable std::array<std::vector<double>, 4> m_t_st_y; // vector of the y components of the simulated hits of the truth particle for each station
  mutable std::array<std::vector<double>, 4> m_t_st_z; // vector of the z components of the simulated hits of the truth particle for each station
  mutable std::array<std::vector<double>, 4> m_t_st_px; // x components of the true momentum at each station
  mutable std::array<std::vector<double>, 4> m_t_st_py; // y components of the true momentum at each station
  mutable std::array<std::vector<double>, 4> m_t_st_pz; // z components of the true momentum at each station
  mutable std::vector<bool> m_isFiducial; // track is fiducial if there are simulated hits for stations 1 - 3 and the distance from the center is smaller than 100 mm

  mutable std::vector<int> m_truthParticleBarcode; // vector of barcodes of all truth particles with a momentum larger 50 GeV
  mutable std::vector<int> m_truthParticleMatchedTracks; // vector of number of tracks to which a truth particle is matched to
  mutable std::vector<bool> m_truthParticleIsFiducial; // vector of boolean showing whether a truth particle is fiducial


  // mother + daughter truth information 

  mutable std::vector<double> m_truthM_P;

  mutable std::vector<double> m_truthM_px;
  mutable std::vector<double> m_truthM_py;
  mutable std::vector<double> m_truthM_pz;

  mutable std::vector<double> m_truthM_x; // decay vertex of A'
  mutable std::vector<double> m_truthM_y;
  mutable std::vector<double> m_truthM_z;

  mutable std::vector<double> m_truthd0_P;

  mutable std::vector<double> m_truthd0_px;
  mutable std::vector<double> m_truthd0_py;
  mutable std::vector<double> m_truthd0_pz;

  mutable std::vector<double> m_truthd0_x; // production vertex for daughter particles 
  mutable std::vector<double> m_truthd0_y;
  mutable std::vector<double> m_truthd0_z;

  mutable std::vector<double> m_truthd1_P;

  mutable std::vector<double> m_truthd1_px;
  mutable std::vector<double> m_truthd1_py;
  mutable std::vector<double> m_truthd1_pz;

  mutable std::vector<double> m_truthd1_x;
  mutable std::vector<double> m_truthd1_y;
  mutable std::vector<double> m_truthd1_z;

  // first 10 truth particles 

  mutable std::vector<double> m_truth_P;
  mutable std::vector<double> m_truth_px;
  mutable std::vector<double> m_truth_py;
  mutable std::vector<double> m_truth_pz;
  mutable std::vector<double> m_truth_theta; 
  mutable std::vector<double> m_truth_phi; 
  mutable std::vector<double> m_truth_pT; 
  mutable std::vector<double> m_truth_eta;
  mutable std::vector<double> m_truth_m;
  mutable std::vector<int> m_truth_barcode;

  mutable std::vector<double> m_truth_dec_x; // components of decay vertex (mm)
  mutable std::vector<double> m_truth_dec_y;
  mutable std::vector<double> m_truth_dec_z;

  mutable std::vector<double> m_truth_prod_x; // components of production vertex (mm)
  mutable std::vector<double> m_truth_prod_y;
  mutable std::vector<double> m_truth_prod_z;

  mutable std::vector<int> m_truth_pdg; // pdg of first 10 truth particles 

  mutable double m_truthLeptonMomentum; 
  mutable int    m_truthBarcode;
  mutable int    m_truthPdg;
  mutable double m_crossSection;
  mutable std::vector<double>  m_genWeights;


  mutable int    m_eventsPassed = 0;
  mutable int    m_eventsFailedGRL = 0;

  mutable double m_vertex_x; // components of reconstructed vertex in mm
  mutable double m_vertex_y;
  mutable double m_vertex_z;
  mutable double m_vertex_chi2; // chi2 value of vertex fit
  mutable double m_vertex_px0; // components of lowest momentum track extrapolated to vertex in MeV
  mutable double m_vertex_py0;
  mutable double m_vertex_pz0;
  mutable double m_vertex_p0;
  mutable double m_vertex_px1; // components of second lowest momentum track extrapolated to vertex in MeV
  mutable double m_vertex_py1;
  mutable double m_vertex_pz1;
  mutable double m_vertex_p1;
};

inline const ServiceHandle <ITHistSvc> &NtupleDumperAlg::histSvc() const {
  return m_histSvc;
}

#endif  // NTUPLEDUMPER_NTUPLEDUMPERALG_H
