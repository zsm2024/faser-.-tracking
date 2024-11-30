#ifndef TRAJECTORYWRITERTOOL_H
#define TRAJECTORYWRITERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"

class TFile;
class TTree;
struct FaserActsRecMultiTrajectory;
using TrajectoriesContainer = std::vector<FaserActsRecMultiTrajectory>;

class TrajectoryWriterTool : public AthAlgTool {
 public:
  TrajectoryWriterTool(const std::string& type, 
      const std::string& name, const IInterface* parent);
  
  virtual ~TrajectoryWriterTool() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  void writeout(TrajectoriesContainer trajectories,
      Acts::GeometryContext geoContext, std::vector<Acts::CurvilinearTrackParameters> traj ) const;
  
  void clearVariables() const;

 private:
  void initializeTree();
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "Trajectories.root", "Output root file for CKF Tracks"};
  Gaudi::Property<std::string> m_treeName{this, "TreeName", "tree", ""};
  TFile* m_file;
  TTree* m_tree;

  mutable int m_eventNumber;
  mutable int m_trajNumber;
  mutable int m_trackNumber;

  mutable bool m_hasFittedParams{false};
  mutable std::vector<bool> m_prt;
  mutable std::vector<bool> m_flt;
  mutable std::vector<bool> m_smt;

  mutable int m_nPredicted{0};
  mutable int m_nFiltered{0};
  mutable int m_nSmoothed{0};

  mutable int m_nMeasurements{0};
  mutable int m_nStates{0};
  mutable int m_nOutliers{0};
  mutable int m_nHoles{0};
  mutable float m_chi2_fit{-99.};
  mutable float m_ndf_fit{-99.};

  mutable std::vector<int> m_volumeID;
  mutable std::vector<int> m_layerID;
  mutable std::vector<int> m_moduleID;
  mutable std::vector<float> m_lx_hit;
  mutable std::vector<float> m_ly_hit;
  mutable std::vector<float> m_x_hit;
  mutable std::vector<float> m_y_hit;
  mutable std::vector<float> m_z_hit;
  mutable std::vector<float> m_meas_eLOC0;
  mutable std::vector<float> m_meas_eLOC1;
  mutable std::vector<float> m_meas_cov_eLOC0;
  mutable std::vector<float> m_meas_cov_eLOC1;
  mutable std::vector<float> m_res_x_hit;
  mutable std::vector<float> m_res_y_hit;
  mutable std::vector<float> m_err_x_hit;
  mutable std::vector<float> m_err_y_hit;
  mutable std::vector<float> m_pull_x_hit;
  mutable std::vector<float> m_pull_y_hit;
  mutable std::vector<int> m_dim_hit;

  mutable int m_charge_fit{-99};
  mutable float m_eLOC0_fit{-99.};
  mutable float m_eLOC1_fit{-99.};
  mutable float m_ePHI_fit{-99.};
  mutable float m_eTHETA_fit{-99.};
  mutable float m_eQOP_fit{-99.};
  mutable float m_eT_fit{-99.};
  mutable float m_err_eLOC0_fit{-99.};
  mutable float m_err_eLOC1_fit{-99.};
  mutable float m_err_ePHI_fit{-99.};
  mutable float m_err_eTHETA_fit{-99.};
  mutable float m_err_eQOP_fit{-99.};
  mutable float m_err_eT_fit{-99.};
  mutable float m_px_fit{-99.};
  mutable float m_py_fit{-99.};
  mutable float m_pz_fit{-99.};
  mutable float m_x_fit{-99.};
  mutable float m_y_fit{-99.};
  mutable float m_z_fit{-99.};

  mutable std::vector<float> m_eLOC0_prt;
  mutable std::vector<float> m_eLOC1_prt;
  mutable std::vector<float> m_ePHI_prt;
  mutable std::vector<float> m_eTHETA_prt;
  mutable std::vector<float> m_eQOP_prt;
  mutable std::vector<float> m_eT_prt;
  mutable std::vector<float> m_res_eLOC0_prt;
  mutable std::vector<float> m_res_eLOC1_prt;
  mutable std::vector<float> m_err_eLOC0_prt;
  mutable std::vector<float> m_err_eLOC1_prt;
  mutable std::vector<float> m_err_ePHI_prt;
  mutable std::vector<float> m_err_eTHETA_prt;
  mutable std::vector<float> m_err_eQOP_prt;
  mutable std::vector<float> m_err_eT_prt;
  mutable std::vector<float> m_pull_eLOC0_prt;
  mutable std::vector<float> m_pull_eLOC1_prt;
  mutable std::vector<float> m_x_prt;
  mutable std::vector<float> m_y_prt;
  mutable std::vector<float> m_z_prt;
  mutable std::vector<float> m_px_prt;
  mutable std::vector<float> m_py_prt;
  mutable std::vector<float> m_pz_prt;
  mutable std::vector<float> m_eta_prt;
  mutable std::vector<float> m_pT_prt;

  mutable std::vector<float> m_eLOC0_flt;
  mutable std::vector<float> m_eLOC1_flt;
  mutable std::vector<float> m_ePHI_flt;
  mutable std::vector<float> m_eTHETA_flt;
  mutable std::vector<float> m_eQOP_flt;
  mutable std::vector<float> m_eT_flt;
  mutable std::vector<float> m_res_eLOC0_flt;
  mutable std::vector<float> m_res_eLOC1_flt;
  mutable std::vector<float> m_err_eLOC0_flt;
  mutable std::vector<float> m_err_eLOC1_flt;
  mutable std::vector<float> m_err_ePHI_flt;
  mutable std::vector<float> m_err_eTHETA_flt;
  mutable std::vector<float> m_err_eQOP_flt;
  mutable std::vector<float> m_err_eT_flt;
  mutable std::vector<float> m_pull_eLOC0_flt;
  mutable std::vector<float> m_pull_eLOC1_flt;
  mutable std::vector<float> m_x_flt;
  mutable std::vector<float> m_y_flt;
  mutable std::vector<float> m_z_flt;
  mutable std::vector<float> m_px_flt;
  mutable std::vector<float> m_py_flt;
  mutable std::vector<float> m_pz_flt;
  mutable std::vector<float> m_eta_flt;
  mutable std::vector<float> m_pT_flt;
  mutable std::vector<float> m_chi2;

  mutable std::vector<float> m_eLOC0_smt;
  mutable std::vector<float> m_eLOC1_smt;
  mutable std::vector<float> m_ePHI_smt;
  mutable std::vector<float> m_eTHETA_smt;
  mutable std::vector<float> m_eQOP_smt;
  mutable std::vector<float> m_eT_smt;
  mutable std::vector<float> m_res_eLOC0_smt;
  mutable std::vector<float> m_res_eLOC1_smt;
  mutable std::vector<float> m_err_eLOC0_smt;
  mutable std::vector<float> m_err_eLOC1_smt;
  mutable std::vector<float> m_err_ePHI_smt;
  mutable std::vector<float> m_err_eTHETA_smt;
  mutable std::vector<float> m_err_eQOP_smt;
  mutable std::vector<float> m_err_eT_smt;
  mutable std::vector<float> m_pull_eLOC0_smt;
  mutable std::vector<float> m_pull_eLOC1_smt;
  mutable std::vector<float> m_x_smt;
  mutable std::vector<float> m_y_smt;
  mutable std::vector<float> m_z_smt;
  mutable std::vector<float> m_px_smt;
  mutable std::vector<float> m_py_smt;
  mutable std::vector<float> m_pz_smt;
  mutable std::vector<float> m_eta_smt;
  mutable std::vector<float> m_pT_smt;

  mutable std::vector<float> m_eLOC0_ini;
  mutable std::vector<float> m_eLOC1_ini;
  mutable std::vector<float> m_ePHI_ini;
  mutable std::vector<float> m_eTHETA_ini;
  mutable std::vector<float> m_eQOP_ini;
  mutable std::vector<float> m_eT_ini;
  mutable std::vector<float> m_err_eLOC0_ini;
  mutable std::vector<float> m_err_eLOC1_ini;
  mutable std::vector<float> m_err_ePHI_ini;
  mutable std::vector<float> m_err_eTHETA_ini;
  mutable std::vector<float> m_err_eQOP_ini;
  mutable std::vector<float> m_err_eT_ini;
  mutable std::vector<float> m_x_ini;
  mutable std::vector<float> m_y_ini;
  mutable std::vector<float> m_z_ini;
  mutable std::vector<float> m_px_ini;
  mutable std::vector<float> m_py_ini;
  mutable std::vector<float> m_pz_ini;
  mutable std::vector<float> m_eta_ini;
  mutable std::vector<float> m_pT_ini;
};

#endif // TRAJECTORYWRITERTOOL_H
