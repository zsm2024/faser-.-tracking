#ifndef NEUTRINOSEARCH_NEUTRINOSEARCHALG_H
#define NEUTRINOSEARCH_NEUTRINOSEARCHALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHit.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "ScintSimEvent/ScintHitCollection.h" 
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"

class TTree;
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

class NeutrinoSearchAlg : public AthReentrantAlgorithm, AthHistogramming {
public:
  NeutrinoSearchAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~NeutrinoSearchAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;
  const ServiceHandle <ITHistSvc> &histSvc() const;

private:

  bool waveformHitOK(const xAOD::WaveformHit* hit) const;
  void clearTree() const;

  ServiceHandle <ITHistSvc> m_histSvc;

  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventContainer { this, "EventContainer", "TruthEvents", "Truth event container name." };
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainer { this, "ParticleContainer", "TruthParticles", "Truth particle container name." };
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollection {this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  SG::ReadHandleKey<ScintHitCollection> m_vetoNuHitKey { this, "VetoNuHitCollection", "VetoNuHits" };

  SG::ReadHandleKey<TrackCollection> m_trackCollection { this, "TrackCollection", "CKFTrackCollection", "Input track collection name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_vetoNuContainer { this, "VetoNuContainer", "VetoNuWaveformHits", "VetoNu hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_vetoContainer { this, "VetoContainer", "VetoWaveformHits", "Veto hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_triggerContainer { this, "TriggerContainer", "TriggerWaveformHits", "Trigger hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_preshowerContainer { this, "PreshowerContainer", "PreshowerWaveformHits", "Preshower hit container name" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_ecalContainer { this, "EcalContainer", "CaloWaveformHits", "Ecal hit container name" };
  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_clusterContainer { this, "ClusterContainer", "SCT_ClusterContainer", "Tracker cluster container name" };

  ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool { this, "ExtrapolationTool", "FaserActsExtrapolationTool" };  
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  const TrackerDD::SCT_DetectorManager* m_detMgr {nullptr};

  const FaserSCT_ID* m_sctHelper;
  const VetoNuID*    m_vetoNuHelper;
  const VetoID*      m_vetoHelper;
  const TriggerID*   m_triggerHelper;
  const PreshowerID* m_preshowerHelper;
  const EcalID*      m_ecalHelper;

  // TODO: use realistic thresholds for MIP
//   DoubleProperty m_vetoNuThreshold    { this, "VetoNuThreshold",    0, "Threshold for VetoNu pmts" };
//   DoubleProperty m_vetoThreshold      { this, "VetoThreshold",      0, "Threshold for Veto pmts" };
//   DoubleProperty m_triggerThreshold   { this, "TriggerThreshold",   0, "Threshold for Trigger pmts" };
//   DoubleProperty m_preshowerThreshold { this, "PreshowerThreshold", 0, "Threshold for Preshower pmts" };
//   DoubleProperty m_ecalThreshold      { this, "EcalThreshold",      0, "Threshold for Ecal pmts" };
  IntegerProperty m_minTrackerLayers     { this, "MinTrackerLayers", 7, "Minimum number of layers with hits on track" };

  BooleanProperty m_useFlukaWeights   { this, "UseFlukaWeights", false, "Flag to weight events according to value stored in HepMC::GenEvent" };
  BooleanProperty m_useGenieWeights   { this, "UseGenieWeights", false, "Flag to weight events according to Genie luminosity" };
  IntegerProperty m_flukaCollisions   { this, "FlukaCollisions", 137130000, "Number of proton-proton collisions in FLUKA sample." };
  DoubleProperty  m_flukaCrossSection { this, "FlukaCrossSection", 80.0, "Fluka p-p inelastic cross-section in millibarns." };
  DoubleProperty  m_genieLuminosity   { this, "GenieLuminosity", 600.0, "Genie luminosity in inverse fb." };
  DoubleProperty  m_zExtrapolate      { this, "ZExtrapolationPlane", -3112.0, "Plane to extrapolate tracks for VetoNu" };

//   BooleanProperty m_enforceBlinding   { this, "EnforceBlinding", true, "Ignore data events with no VetoNu signals." };
  const bool m_enforceBlinding {true};

  double m_baseEventCrossSection {1.0};
  const double kfemtoBarnsPerMilliBarn {1.0e12};

  mutable TTree* m_tree;
  mutable unsigned int m_run_number;
  mutable unsigned int m_event_number;
  mutable double m_vetoNu0;
  mutable double m_vetoNu1;
  mutable double m_veto00;
  mutable double m_veto01;
  mutable double m_vetoUpstream;
  mutable double m_veto10;
  mutable double m_veto11;
  mutable double m_vetoDownstream;
  mutable double m_trigger00;
  mutable double m_trigger01;
  mutable double m_trigger10;
  mutable double m_trigger11;
  mutable double m_triggerTotal;
  mutable double m_preshower0;
  mutable double m_preshower1;
  mutable double m_ecal00;
  mutable double m_ecal01;
  mutable double m_ecal10;
  mutable double m_ecal11;
  mutable double m_ecalTotal;
  mutable int m_station0Clusters;
  mutable int m_station1Clusters;
  mutable int m_station2Clusters;
  mutable int m_station3Clusters;

  // mutable double m_vetoNuHitsMeanX;
  // mutable double m_vetoNuHitsMeanY;

  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
  mutable double m_px;
  mutable double m_py;
  mutable double m_pz;
  mutable double m_p;
  mutable int    m_charge;
  mutable double m_chi2;
  mutable int    m_ndof;
  mutable int    m_longTracks;
  mutable double m_truthLeptonMomentum;
  mutable double m_truthLeptonX;
  mutable double m_truthLeptonY;
  mutable int    m_truthBarcode;
  mutable int    m_truthPdg;
  mutable double m_crossSection;
  mutable double m_xVetoNu;
  mutable double m_yVetoNu;
  mutable double m_sxVetoNu;
  mutable double m_syVetoNu;
  mutable double m_thetaxVetoNu;
  mutable double m_thetayVetoNu;
};

inline const ServiceHandle <ITHistSvc> &NeutrinoSearchAlg::histSvc() const {
  return m_histSvc;
}

#endif  // NEUTRINOSEARCH_NEUTRINOSEARCHALG_H
