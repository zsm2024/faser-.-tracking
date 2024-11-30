#ifndef FASERACTSKALMANFILTER_ROOTTRAJECTORYSTATESWRITERTOOL_H
#define FASERACTSKALMANFILTER_ROOTTRAJECTORYSTATESWRITERTOOL_H

#include "FaserActsTrack.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "GeneratorObjects/McEventCollection.h"
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
using TrajectoriesContainer = std::vector<FaserActsRecMultiTrajectory>;

class RootTrajectoryStatesWriterTool : public AthAlgTool {
public:
  RootTrajectoryStatesWriterTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~RootTrajectoryStatesWriterTool() override = default;

  StatusCode initialize() override;
  StatusCode finalize() override;

  StatusCode write(const Acts::GeometryContext& gctx, const FaserActsTrackContainer& tracks, bool isMC) const;

private:
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey {this, "McEventCollection", "TruthEvent"};
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  SG::ReadHandleKey <FaserSiHitCollection> m_faserSiHitKey {this, "FaserSiHitCollection", "SCT_Hits"};

  const double m_MeV2GeV = 0.001;

  const FaserSCT_ID* m_idHelper{nullptr};
  const TrackerDD::SCT_DetectorManager* m_detMgr {nullptr};
  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performance writer, states writer or summary writer"};
  Gaudi::Property<std::string> m_filePath {this, "FilePath", "track_states_ckf.root", "Output root file"};
  Gaudi::Property<std::string> m_treeName {this, "TreeName", "tree", "Tree name"};
  Gaudi::Property<bool> m_mc {this, "MC", false};
  TFile* m_outputFile;
  TTree* m_outputTree;

  enum ParameterType { ePredicted, eFiltered, eSmoothed, eUnbiased, eSize };

  mutable uint32_t m_eventNr{0};         ///< the event number
  mutable uint32_t m_trackNr{0};  ///< The track number in event

  mutable std::vector<float> m_t_x;  ///< Global truth hit position x
  mutable std::vector<float> m_t_y;  ///< Global truth hit position y
  mutable std::vector<float> m_t_z;  ///< Global truth hit position z
  mutable std::vector<float> m_t_dx;  ///< Truth particle direction x at global hit position
  mutable std::vector<float> m_t_dy;  ///< Truth particle direction y at global hit position
  mutable std::vector<float> m_t_dz;  ///< Truth particle direction z at global hit position

  mutable std::vector<float> m_t_eLOC0;   ///< truth parameter eBoundLoc0
  mutable std::vector<float> m_t_eLOC1;   ///< truth parameter eBoundLoc1
  mutable std::vector<float> m_t_ePHI;    ///< truth parameter ePHI
  mutable std::vector<float> m_t_eTHETA;  ///< truth parameter eTHETA
  mutable std::vector<float> m_t_eQOP;    ///< truth parameter eQOP
  mutable std::vector<float> m_t_eT;      ///< truth parameter eT
  mutable double m_t_rMax{0}; /// max radius of the truth hits on the track 

  mutable unsigned int m_nStates{0};        ///< number of all states
  mutable unsigned int m_nMeasurements{0};  ///< number of states with measurements
  mutable unsigned int m_nOutliers{0};  ///< number of outliers 
  mutable unsigned int m_nHoles{0};  ///< number of holes 
  mutable double m_chi2Sum{0};  ///< chisq 
  mutable unsigned int m_NDF{0};  ///< number of degree of freedom 
  mutable double m_rMax_hit{0}; /// max radius of the measurements on the track  
  mutable std::vector<int> m_volumeID;      ///< volume identifier
  mutable std::vector<int> m_layerID;       ///< layer identifier
  mutable std::vector<int> m_moduleID;      ///< surface identifier
  mutable std::vector<int> m_station;       ///< station
  mutable std::vector<int> m_layer;         ///< layer
  mutable std::vector<int> m_phi_module;    ///< phi module (rows)
  mutable std::vector<int> m_eta_module;    ///< eta module (columns)
  mutable std::vector<int> m_side;          ///< module side
  mutable std::vector<float> m_pathLength;  ///< path length
  mutable std::vector<float> m_lx_hit;      ///< uncalibrated measurement local x
  mutable std::vector<float> m_ly_hit;      ///< uncalibrated measurement local y
  mutable std::vector<float> m_x_hit;       ///< uncalibrated measurement global x
  mutable std::vector<float> m_y_hit;       ///< uncalibrated measurement global y
  mutable std::vector<float> m_z_hit;       ///< uncalibrated measurement global z
  mutable std::vector<float> m_res_x_hit;   ///< hit residual x
  mutable std::vector<float> m_res_y_hit;   ///< hit residual y
  mutable std::vector<float> m_err_x_hit;   ///< hit err x
  mutable std::vector<float> m_err_y_hit;   ///< hit err y
  mutable std::vector<float> m_pull_x_hit;  ///< hit pull x
  mutable std::vector<float> m_pull_y_hit;  ///< hit pull y
  mutable std::vector<int> m_dim_hit;       ///< dimension of measurement
  mutable std::vector<int> m_index_hit; /// hit index

  mutable std::array<int, eSize> m_nParams;  ///< number of states which have filtered/predicted/smoothed parameters
  mutable std::array<double, eSize> m_rMax;  ///< max radius of the predicted/filtered/smoothed parameters on the track 
  mutable std::array<std::vector<bool>, eSize> m_hasParams;  ///< status of the filtered/predicted/smoothed parameters
  mutable std::array<std::vector<float>, eSize> m_eLOC0;  ///< predicted/filtered/smoothed parameter eLOC0
  mutable std::array<std::vector<float>, eSize> m_eLOC1;  ///< predicted/filtered/smoothed parameter eLOC1
  mutable std::array<std::vector<float>, eSize> m_ePHI;  ///< predicted/filtered/smoothed parameter ePHI
  mutable std::array<std::vector<float>, eSize> m_eTHETA;  ///< predicted/filtered/smoothed parameter eTHETA
  mutable std::array<std::vector<float>, eSize> m_eQOP;  ///< predicted/filtered/smoothed parameter eQOP
  mutable std::array<std::vector<float>, eSize> m_eT;  ///< predicted/filtered/smoothed parameter eT
  mutable std::array<std::vector<float>, eSize> m_res_eLOC0;  ///< predicted/filtered/smoothed parameter eLOC0 residual
  mutable std::array<std::vector<float>, eSize> m_res_eLOC1;  ///< predicted/filtered/smoothed parameter eLOC1 residual
  mutable std::array<std::vector<float>, eSize> m_res_ePHI;  ///< predicted/filtered/smoothed parameter ePHI residual
  mutable std::array<std::vector<float>, eSize> m_res_eTHETA;  ///< predicted/filtered/smoothed parameter eTHETA residual
  mutable std::array<std::vector<float>, eSize> m_res_eQOP;  ///< predicted/filtered/smoothed parameter eQOP residual
  mutable std::array<std::vector<float>, eSize> m_res_eT;  ///< predicted/filtered/smoothed parameter eT residual
  mutable std::array<std::vector<float>, eSize> m_err_eLOC0;  ///< predicted/filtered/smoothed parameter eLOC0 error
  mutable std::array<std::vector<float>, eSize> m_err_eLOC1;  ///< predicted/filtered/smoothed parameter eLOC1 error
  mutable std::array<std::vector<float>, eSize> m_err_ePHI;  ///< predicted/filtered/smoothed parameter ePHI error
  mutable std::array<std::vector<float>, eSize> m_err_eTHETA;  ///< predicted/filtered/smoothed parameter eTHETA error
  mutable std::array<std::vector<float>, eSize> m_err_eQOP;  ///< predicted/filtered/smoothed parameter eQOP error
  mutable std::array<std::vector<float>, eSize> m_err_eT;  ///< predicted/filtered/smoothed parameter eT error
  mutable std::array<std::vector<float>, eSize> m_pull_eLOC0;  ///< predicted/filtered/smoothed parameter eLOC0 pull
  mutable std::array<std::vector<float>, eSize> m_pull_eLOC1;  ///< predicted/filtered/smoothed parameter eLOC1 pull
  mutable std::array<std::vector<float>, eSize> m_pull_ePHI;  ///< predicted/filtered/smoothed parameter ePHI pull
  mutable std::array<std::vector<float>, eSize> m_pull_eTHETA;  ///< predicted/filtered/smoothed parameter eTHETA pull
  mutable std::array<std::vector<float>, eSize> m_pull_eQOP;  ///< predicted/filtered/smoothed parameter eQOP pull
  mutable std::array<std::vector<float>, eSize> m_pull_eT;  ///< predicted/filtered/smoothed parameter eT pull
  mutable std::array<std::vector<float>, eSize> m_x;  ///< predicted/filtered/smoothed parameter global x
  mutable std::array<std::vector<float>, eSize> m_y;  ///< predicted/filtered/smoothed parameter global y
  mutable std::array<std::vector<float>, eSize> m_z;  ///< predicted/filtered/smoothed parameter global z
  mutable std::array<std::vector<float>, eSize> m_px;  ///< predicted/filtered/smoothed parameter px
  mutable std::array<std::vector<float>, eSize> m_py;  ///< predicted/filtered/smoothed parameter py
  mutable std::array<std::vector<float>, eSize> m_pz;  ///< predicted/filtered/smoothed parameter pz
  mutable std::array<std::vector<float>, eSize> m_eta;  ///< predicted/filtered/smoothed parameter eta
  mutable std::array<std::vector<float>, eSize> m_pT;  ///< predicted/filtered/smoothed parameter pT

  mutable  std::vector<float> m_chi2;  ///< chisq from filtering
};

#endif  // FASERACTSKALMANFILTER_ROOTTRAJECTORYSTATESWRITERTOOL_H
