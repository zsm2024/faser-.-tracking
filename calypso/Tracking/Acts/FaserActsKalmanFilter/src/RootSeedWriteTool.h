#ifndef ROOTSEEDWRITETOOL_H
#define ROOTSEEDWRITETOOL_H
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include"AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
//#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "GeneratorObjects/McEventCollection.h"

//#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
//#include "xAODEventInfo/EventInfo.h"

#include "RootTrajectorySummaryWriterTool.h"
#include"CircleFitTrackSeedTool.h"
#include<string>
#include<vector>

class TFile;
class TTree;


class RootSeedWriterTool : public AthAlgTool {
public:
  RootSeedWriterTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~RootSeedWriterTool() override = default;
  StatusCode write(const Acts::GeometryContext& geoContext, const std::vector<CircleFitTrackSeedTool::Seed> &seeds, bool isMC) const;
  StatusCode initialize() override;
  StatusCode finalize() override;
private:
  ToolHandle<RootTrajectorySummaryWriterTool> m_summaryTool {
      this, "summaryTool","RootTrajectorySummaryWriterTool"};
  std::optional<const Acts::BoundTrackParameters> extrapolateToReferenceSurface(
    const EventContext& ctx, const HepMC::GenParticle* particle) const;
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {
    this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey {
    this, "McEventCollection", "TruthEvent"};
//  SG::ReadHandleKey<xAOD::TruthEventContainer> m_truthEventContainer { this, "EventContainer", "TruthEvents", "Truth event container name." };
  SG::ReadHandleKey<xAOD::TruthParticleContainer> m_truthParticleContainer { this, "ParticleContainer", "TruthParticles", "Truth particle container name." };
 // SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfoKey", "EventInfo", "ReadHandleKey for EventInfo"};

  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performents writer,seed writer, states writer or summary writer"};
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "seed_summary_circleFitTrackSeedTool.root", "Output root file"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "tree", "Tree name"};
  Gaudi::Property<bool> m_mc {this, "MC", false};

  
  TFile* m_outputFile;
  TTree* m_outputTree;

  mutable std::vector<double> m_truth_pdg;
  mutable std::vector<double> m_truth_barcode;
  mutable std::vector<double> m_truth_P;
  mutable std::vector<double> m_truth_px;
  mutable std::vector<double> m_truth_py;
  mutable std::vector<double> m_truth_pz;
  mutable std::vector<double> m_truth_pT;
  mutable std::vector<double> m_truth_theta;
  mutable std::vector<double> m_truth_phi;
  mutable std::vector<double> m_truth_eta;

  mutable uint32_t m_eventNr{0};
  mutable std::vector<unsigned int> m_nClusters;
  mutable std::vector<unsigned int> m_nMeasurements;
  mutable uint32_t m_nseeds{0};
  mutable std::vector<unsigned int> m_nSegments;
  mutable std::vector<double> m_positionX;
  mutable std::vector<double> m_positionY;
  mutable std::vector<double> m_positionZ;
  mutable std::vector<double> m_spacePointX;
  mutable std::vector<double> m_spacePointY;
  mutable std::vector<double> m_spacePointZ;
  mutable std::vector<double> m_fakePositionX;
  mutable std::vector<double> m_fakePositionY;
  mutable std::vector<double> m_fakePositionZ;

  mutable std::vector<double> m_nMajorityHits;  ///< The number of hits from majority particle
  mutable std::vector<uint64_t> m_majorityParticleId;      ///< The particle Id of the majority particle
  mutable std::vector<double> m_t_truthHitRatio;  ///< the purity of every track
  mutable std::vector<int> m_t_charge;   ///< Charge of majority particle
  mutable std::vector<float> m_t_time;   ///< Time of majority particle
  mutable std::vector<float> m_t_vx;     ///< Vertex x positions of majority particle
  mutable std::vector<float> m_t_vy;     ///< Vertex y positions of majority particle
  mutable std::vector<float> m_t_vz;     ///< Vertex z positions of majority particle
  mutable std::vector<float> m_t_px;     ///< Initial momenta px of majority particle
  mutable std::vector<float> m_t_py;     ///< Initial momenta py of majority particle
  mutable std::vector<float> m_t_pz;     ///< Initial momenta pz of majority particle
  mutable std::vector<float> m_t_theta;  ///< In * m_MeV2GeVitial momenta theta of majority particle
  mutable std::vector<float> m_t_phi;    ///< Initial momenta phi of majority particle
  mutable std::vector<float> m_t_p;      ///< Initial abs momenta of majority particle
  mutable std::vector<float> m_t_pT;     ///< Initial momenta pT of majority particle
  mutable std::vector<float> m_t_eta;    ///< Initial momenta eta of majority particle
};
#endif
