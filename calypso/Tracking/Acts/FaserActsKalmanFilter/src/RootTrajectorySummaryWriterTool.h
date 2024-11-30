#ifndef FASERACTSKALMANFILTER_ROOTTRAJECTORYSUMMARYWRITERTOOL_H
#define FASERACTSKALMANFILTER_ROOTTRAJECTORYSUMMARYWRITERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "FaserActsKalmanFilter/IdentifierLink.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "GeneratorObjects/McEventCollection.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "FaserActsTrack.h"
#include <array>
#include <string>
#include <vector>

class FaserSCT_ID;
namespace  TrackerDD {
class SCT_DetectorManager;
}
class TFile;
class TTree;
struct FaserActsRecMultiTrajectory;

class RootTrajectorySummaryWriterTool : public AthAlgTool {
public:
  RootTrajectorySummaryWriterTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~RootTrajectorySummaryWriterTool() override = default;
  StatusCode write( const Acts::GeometryContext& geoContext, const FaserActsTrackContainer& tracks, bool isMC) const;
  StatusCode initialize() override;
  StatusCode finalize() override;
  std::optional<const Acts::BoundTrackParameters> extrapolateToReferenceSurface(
      const EventContext& ctx, const HepMC::GenParticle* particle) const;
private:
//  std::optional<const Acts::BoundTrackParameters> extrapolateToReferenceSurface(
  //    const EventContext& ctx, const HepMC::GenParticle* particle) const;
  const FaserSCT_ID* m_idHelper {nullptr};
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {
    this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey {
    this, "McEventCollection", "TruthEvent"};
  ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool {
      this, "ExtrapolationTool", "FaserActsExtrapolationTool"};

  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performance writer, states writer or summary writer"};
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "track_summary_ckf.root", "Output root file"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "tree", "Tree name"};

  const double m_MeV2GeV = 0.001;

  TFile* m_outputFile;
  TTree* m_outputTree;
  mutable uint32_t m_eventNr{0};         ///< The event number
  mutable std::vector<uint32_t> m_trackNr;  ///< The track number in event

  mutable std::vector<unsigned int> m_nStates;        ///< The number of states
  mutable std::vector<unsigned int> m_nMeasurements;  ///< The number of measurements
  mutable std::vector<unsigned int> m_nOutliers;      ///< The number of outliers
  mutable std::vector<unsigned int> m_nHoles;         ///< The number of holes
  mutable std::vector<unsigned int> m_nSharedHits;    ///< The number of shared hits
  mutable std::vector<float> m_chi2Sum;               ///< The total chi2
  mutable std::vector<unsigned int> m_NDF;  ///< The number of ndf of the measurements+outliers
  mutable std::vector<double> m_rMax_hit;  ///< max radius of the measurements on the track 
  mutable std::vector<std::vector<double>> m_measurementChi2;  ///< The chi2 on all measurement states
  mutable std::vector<std::vector<double>> m_outlierChi2;  ///< The chi2 on all outlier states
  // FIXME replace volume, layer, ... with station, layer, ...
  mutable std::vector<std::vector<double>> m_measurementVolume;  ///< The volume id of the measurements
  mutable std::vector<std::vector<double>> m_measurementLayer;  ///< The layer id of the measurements
  mutable std::vector<std::vector<int>> m_measurementIndex;  ///< The hit id of the measurements
  mutable std::vector<std::vector<double>> m_outlierVolume;  ///< The volume id of the outliers
  mutable std::vector<std::vector<double>> m_outlierLayer;  ///< The layer id of the outliers
  mutable std::vector<std::vector<int>> m_outlierIndex;  ///< The hit id of the outliers

  // The majority truth particle info
  mutable std::vector<unsigned int> m_nMajorityHits;  ///< The number of hits from majority particle
  mutable std::vector<uint64_t> m_majorityParticleId;      ///< The particle Id of the majority particle
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
  mutable std::vector<float> m_t_d0;  ///< The extrapolated truth transverse impact parameter
  mutable std::vector<float> m_t_z0;  ///< The extrapolated truth longitudinal impact parameter

  mutable std::vector<bool> m_hasFittedParams;  ///< If the track has fitted parameter
  //The fitted parameters representing surface center 
  mutable std::vector<float> m_surface_x;   ///< Fitted parameters representing surface x of track
  mutable std::vector<float> m_surface_y;   ///< Fitted parameters representing surface y of track
  mutable std::vector<float> m_surface_z;   ///< Fitted parameters representing surface z of track
  // The fitted parameters
  mutable std::vector<float> m_eLOC0_fit;   ///< Fitted parameters eBoundLoc0 of track
  mutable std::vector<float> m_eLOC1_fit;   ///< Fitted parameters eBoundLoc1 of track
  mutable std::vector<float> m_ePHI_fit;    ///< Fitted parameters ePHI of track
  mutable std::vector<float> m_eTHETA_fit;  ///< Fitted parameters eTHETA of track
  mutable std::vector<float> m_eQOP_fit;    ///< Fitted parameters eQOP of track
  mutable std::vector<float> m_eT_fit;      ///< Fitted parameters eT of track
  // The error of fitted parameters
  mutable std::vector<float> m_err_eLOC0_fit;   ///< Fitted parameters eLOC err of track
  mutable std::vector<float> m_err_eLOC1_fit;   ///< Fitted parameters eBoundLoc1 err of track
  mutable std::vector<float> m_err_ePHI_fit;    ///< Fitted parameters ePHI err of track
  mutable std::vector<float> m_err_eTHETA_fit;  ///< Fitted parameters eTHETA err of track
  mutable std::vector<float> m_err_eQOP_fit;    ///< Fitted parameters eQOP err of track
  mutable std::vector<float> m_err_eT_fit;      ///< Fitted parameters eT err of track
  // The residual of fitted parameters
  mutable std::vector<float> m_res_eLOC0_fit;   ///< Fitted parameters eLOC res of track
  mutable std::vector<float> m_res_eLOC1_fit;   ///< Fitted parameters eBoundLoc1 res of track
  mutable std::vector<float> m_res_ePHI_fit;    ///< Fitted parameters ePHI res of track
  mutable std::vector<float> m_res_eTHETA_fit;  ///< Fitted parameters eTHETA res of track
  mutable std::vector<float> m_res_eQOP_fit;    ///< Fitted parameters eQOP res of track
  mutable std::vector<float> m_res_eT_fit;      ///< Fitted parameters eT res of track
  // The pull of fitted parameters
  mutable std::vector<float> m_pull_eLOC0_fit;    ///< Fitted parameters eLOC pull of track
  mutable std::vector<float> m_pull_eLOC1_fit;    ///< Fitted parameters eBoundLoc1 pull of track
  mutable std::vector<float> m_pull_ePHI_fit;    ///< Fitted parameters ePHI pull of track
  mutable std::vector<float> m_pull_eTHETA_fit;  ///< Fitted parameters eTHETA pull of track
  mutable std::vector<float> m_pull_eQOP_fit;    ///< Fitted parameters eQOP pull of track
  mutable std::vector<float> m_pull_eT_fit;      ///< Fitted parameters eT pull of track
};

#endif  // FASERACTSKALMANFILTER_ROOTTRAJECTORYSUMMARYWRITERTOOL_H
