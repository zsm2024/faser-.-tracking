#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrajectoryWriterTool.h"
#include "FaserActsRecMultiTrajectory.h"
#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "TFile.h"
#include "TTree.h"

using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::perp;
using Acts::VectorHelpers::phi;
using Acts::VectorHelpers::theta;


TrajectoryWriterTool::TrajectoryWriterTool(const std::string& type, 
    const std::string& name, const IInterface* parent)
    : AthAlgTool( type, name, parent ) {}


StatusCode TrajectoryWriterTool::initialize() {
  ATH_MSG_INFO("TrajectoryWriterTool::initialize");

  std::string filePath = m_filePath;
  std::string treeName = m_treeName;

  m_file = TFile::Open(filePath.c_str(), "RECREATE");
  if (m_file == nullptr) {
    ATH_MSG_ERROR("Unable to open output file at " << m_filePath);
    return StatusCode::FAILURE;
  }
  m_file->cd();
  m_tree = new TTree(treeName.c_str(), "tree");
  if (m_tree == nullptr) {
    ATH_MSG_ERROR("Unable to create TTree");
    return StatusCode::FAILURE;
  }

  TrajectoryWriterTool::initializeTree();

  return StatusCode::SUCCESS;
}


StatusCode TrajectoryWriterTool::finalize() {
  ATH_MSG_INFO("finalize WriteOutInitialParameterTool");

  m_file->cd();
  m_tree->Write();

  return StatusCode::SUCCESS;
}


void TrajectoryWriterTool::initializeTree() {
  m_tree->Branch("event_number", &m_eventNumber);
  m_tree->Branch("traj_number", &m_trajNumber);

  m_tree->Branch("nMeasurements", &m_nMeasurements);
  m_tree->Branch("nStates", &m_nStates);
  m_tree->Branch("nOutliers", &m_nOutliers);
  m_tree->Branch("nHoles", &m_nHoles);

  m_tree->Branch("hasFittedParams", &m_hasFittedParams);
  m_tree->Branch("chi2_fit", &m_chi2_fit);
  m_tree->Branch("ndf_fit", &m_ndf_fit);
  m_tree->Branch("eLOC0_fit", &m_eLOC0_fit);
  m_tree->Branch("eLOC1_fit", &m_eLOC1_fit);
  m_tree->Branch("ePHI_fit", &m_ePHI_fit);
  m_tree->Branch("eTHETA_fit", &m_eTHETA_fit);
  m_tree->Branch("eQOP_fit", &m_eQOP_fit);
  m_tree->Branch("eT_fit", &m_eT_fit);
  m_tree->Branch("charge_fit", &m_charge_fit);
  m_tree->Branch("err_eLOC0_fit", &m_err_eLOC0_fit);
  m_tree->Branch("err_eLOC1_fit", &m_err_eLOC1_fit);
  m_tree->Branch("err_ePHI_fit", &m_err_ePHI_fit);
  m_tree->Branch("err_eTHETA_fit", &m_err_eTHETA_fit);
  m_tree->Branch("err_eQOP_fit", &m_err_eQOP_fit);
  m_tree->Branch("err_eT_fit", &m_err_eT_fit);
  m_tree->Branch("g_px_fit", &m_px_fit);
  m_tree->Branch("g_py_fit", &m_py_fit);
  m_tree->Branch("g_pz_fit", &m_pz_fit);
  m_tree->Branch("g_x_fit" , &m_x_fit);
  m_tree->Branch("g_y_fit" , &m_y_fit);
  m_tree->Branch("g_z_fit" , &m_z_fit);
  m_tree->Branch("lx_hit" , &m_lx_hit);
  m_tree->Branch("ly_hit" , &m_ly_hit);
  m_tree->Branch("x_hit" , &m_x_hit);
  m_tree->Branch("y_hit" , &m_y_hit);
  m_tree->Branch("z_hit" , &m_z_hit);
  m_tree->Branch("meas_eLOC0" , &m_meas_eLOC0);
  m_tree->Branch("meas_eLOC1" , &m_meas_eLOC1);
  m_tree->Branch("meas_cov_eLOC0" , &m_meas_cov_eLOC0);
  m_tree->Branch("meas_cov_eLOC1" , &m_meas_cov_eLOC1);

  m_tree->Branch("nPredicted", &m_nPredicted);
  m_tree->Branch("predicted", &m_prt);
  m_tree->Branch("eLOC0_prt", &m_eLOC0_prt);
  m_tree->Branch("eLOC1_prt", &m_eLOC1_prt);
  m_tree->Branch("ePHI_prt", &m_ePHI_prt);
  m_tree->Branch("eTHETA_prt", &m_eTHETA_prt);
  m_tree->Branch("eQOP_prt", &m_eQOP_prt);
  m_tree->Branch("eT_prt", &m_eT_prt);
  m_tree->Branch("res_eLOC0_prt", &m_res_eLOC0_prt);
  m_tree->Branch("res_eLOC1_prt", &m_res_eLOC1_prt);
  m_tree->Branch("err_eLOC0_prt", &m_err_eLOC0_prt);
  m_tree->Branch("err_eLOC1_prt", &m_err_eLOC1_prt);
  m_tree->Branch("err_ePHI_prt", &m_err_ePHI_prt);
  m_tree->Branch("err_eTHETA_prt", &m_err_eTHETA_prt);
  m_tree->Branch("err_eQOP_prt", &m_err_eQOP_prt);
  m_tree->Branch("err_eT_prt", &m_err_eT_prt);
  m_tree->Branch("pull_eLOC0_prt", &m_pull_eLOC0_prt);
  m_tree->Branch("pull_eLOC1_prt", &m_pull_eLOC1_prt);
  m_tree->Branch("g_x_prt", &m_x_prt);
  m_tree->Branch("g_y_prt", &m_y_prt);
  m_tree->Branch("g_z_prt", &m_z_prt);
  m_tree->Branch("px_prt", &m_px_prt);
  m_tree->Branch("py_prt", &m_py_prt);
  m_tree->Branch("pz_prt", &m_pz_prt);
  m_tree->Branch("eta_prt", &m_eta_prt);
  m_tree->Branch("pT_prt", &m_pT_prt);

  m_tree->Branch("nFiltered", &m_nFiltered);
  m_tree->Branch("filtered", &m_flt);
  m_tree->Branch("eLOC0_flt", &m_eLOC0_flt);
  m_tree->Branch("eLOC1_flt", &m_eLOC1_flt);
  m_tree->Branch("ePHI_flt", &m_ePHI_flt);
  m_tree->Branch("eTHETA_flt", &m_eTHETA_flt);
  m_tree->Branch("eQOP_flt", &m_eQOP_flt);
  m_tree->Branch("eT_flt", &m_eT_flt);
  m_tree->Branch("res_eLOC0_flt", &m_res_eLOC0_flt);
  m_tree->Branch("res_eLOC1_flt", &m_res_eLOC1_flt);
  m_tree->Branch("err_eLOC0_flt", &m_err_eLOC0_flt);
  m_tree->Branch("err_eLOC1_flt", &m_err_eLOC1_flt);
  m_tree->Branch("err_ePHI_flt", &m_err_ePHI_flt);
  m_tree->Branch("err_eTHETA_flt", &m_err_eTHETA_flt);
  m_tree->Branch("err_eQOP_flt", &m_err_eQOP_flt);
  m_tree->Branch("err_eT_flt", &m_err_eT_flt);
  m_tree->Branch("pull_eLOC0_flt", &m_pull_eLOC0_flt);
  m_tree->Branch("pull_eLOC1_flt", &m_pull_eLOC1_flt);
  m_tree->Branch("g_x_flt", &m_x_flt);
  m_tree->Branch("g_y_flt", &m_y_flt);
  m_tree->Branch("g_z_flt", &m_z_flt);
  m_tree->Branch("px_flt", &m_px_flt);
  m_tree->Branch("py_flt", &m_py_flt);
  m_tree->Branch("pz_flt", &m_pz_flt);
  m_tree->Branch("eta_flt", &m_eta_flt);
  m_tree->Branch("pT_flt", &m_pT_flt);
  m_tree->Branch("chi2", &m_chi2);

  m_tree->Branch("nSmoothed", &m_nSmoothed);
  m_tree->Branch("smoothed", &m_smt);
  m_tree->Branch("eLOC0_smt", &m_eLOC0_smt);
  m_tree->Branch("eLOC1_smt", &m_eLOC1_smt);
  m_tree->Branch("ePHI_smt", &m_ePHI_smt);
  m_tree->Branch("eTHETA_smt", &m_eTHETA_smt);
  m_tree->Branch("eQOP_smt", &m_eQOP_smt);
  m_tree->Branch("eT_smt", &m_eT_smt);
  m_tree->Branch("res_eLOC0_smt", &m_res_eLOC0_smt);
  m_tree->Branch("res_eLOC1_smt", &m_res_eLOC1_smt);
  m_tree->Branch("err_eLOC0_smt", &m_err_eLOC0_smt);
  m_tree->Branch("err_eLOC1_smt", &m_err_eLOC1_smt);
  m_tree->Branch("err_ePHI_smt", &m_err_ePHI_smt);
  m_tree->Branch("err_eTHETA_smt", &m_err_eTHETA_smt);
  m_tree->Branch("err_eQOP_smt", &m_err_eQOP_smt);
  m_tree->Branch("err_eT_smt", &m_err_eT_smt);
  m_tree->Branch("pull_eLOC0_smt", &m_pull_eLOC0_smt);
  m_tree->Branch("pull_eLOC1_smt", &m_pull_eLOC1_smt);
  m_tree->Branch("g_x_smt", &m_x_smt);
  m_tree->Branch("g_y_smt", &m_y_smt);
  m_tree->Branch("g_z_smt", &m_z_smt);
  m_tree->Branch("px_smt", &m_px_smt);
  m_tree->Branch("py_smt", &m_py_smt);
  m_tree->Branch("pz_smt", &m_pz_smt);
  m_tree->Branch("eta_smt", &m_eta_smt);
  m_tree->Branch("pT_smt", &m_pT_smt);
  m_tree->Branch("eLOC0_ini", &m_eLOC0_ini);
  m_tree->Branch("eLOC1_ini", &m_eLOC1_ini);
  m_tree->Branch("ePHI_ini", &m_ePHI_ini);
  m_tree->Branch("eTHETA_ini", &m_eTHETA_ini);
  m_tree->Branch("eQOP_ini", &m_eQOP_ini);
  m_tree->Branch("eT_ini", &m_eT_ini);
  m_tree->Branch("err_eLOC0_ini", &m_err_eLOC0_ini);
  m_tree->Branch("err_eLOC1_ini", &m_err_eLOC1_ini);
  m_tree->Branch("err_ePHI_ini", &m_err_ePHI_ini);
  m_tree->Branch("err_eTHETA_ini", &m_err_eTHETA_ini);
  m_tree->Branch("err_eQOP_ini", &m_err_eQOP_ini);
  m_tree->Branch("err_eT_ini", &m_err_eT_ini);
  m_tree->Branch("g_x_ini", &m_x_ini);
  m_tree->Branch("g_y_ini", &m_y_ini);
  m_tree->Branch("g_z_ini", &m_z_ini);
  m_tree->Branch("px_ini", &m_px_ini);
  m_tree->Branch("py_ini", &m_py_ini);
  m_tree->Branch("pz_ini", &m_pz_ini);
  m_tree->Branch("eta_ini", &m_eta_ini);
  m_tree->Branch("pT_ini", &m_pT_ini);
}


void TrajectoryWriterTool::writeout(TrajectoriesContainer trajectories,
    Acts::GeometryContext geoContext,std::vector<Acts::CurvilinearTrackParameters> initialparameters) const {
    
  m_eventNumber = Gaudi::Hive::currentContext().eventID().event_number();


  // Loop over the trajectories
  int iTraj = 0;
  for (const auto& traj : trajectories) {
    m_trajNumber = iTraj;

	//initial paramter
        m_eLOC0_ini.push_back(initialparameters[iTraj].parameters()[Acts::eBoundLoc0]);
        m_eLOC1_ini.push_back(initialparameters[iTraj].parameters()[Acts::eBoundLoc1]);
        m_ePHI_ini.push_back(initialparameters[iTraj].parameters()[Acts::eBoundPhi]);
        m_eTHETA_ini.push_back(initialparameters[iTraj].parameters()[Acts::eBoundTheta]);
        m_eQOP_ini.push_back(initialparameters[iTraj].parameters()[Acts::eBoundQOverP]);
        m_eT_ini.push_back(initialparameters[iTraj].parameters()[Acts::eBoundTime]);

        /// Smoothed parameter uncertainties
	const auto &covariance_ini = *(initialparameters[iTraj]).covariance();
        m_err_eLOC0_ini.push_back(
            sqrt(covariance_ini(Acts::eBoundLoc0, Acts::eBoundLoc0)));
        m_err_eLOC1_ini.push_back(
            sqrt(covariance_ini(Acts::eBoundLoc1, Acts::eBoundLoc1)));
        m_err_ePHI_ini.push_back(
            sqrt(covariance_ini(Acts::eBoundPhi, Acts::eBoundPhi)));
        m_err_eTHETA_ini.push_back(
            sqrt(covariance_ini(Acts::eBoundTheta, Acts::eBoundTheta)));
        m_err_eQOP_ini.push_back(
            sqrt(covariance_ini(Acts::eBoundQOverP, Acts::eBoundQOverP)));
        m_err_eT_ini.push_back(
            sqrt(covariance_ini(Acts::eBoundTime, Acts::eBoundTime)));
        m_x_ini.push_back(initialparameters[iTraj].position(geoContext).x());
        m_y_ini.push_back(initialparameters[iTraj].position(geoContext).y());
        m_z_ini.push_back(initialparameters[iTraj].position(geoContext).z());
        m_px_ini.push_back(initialparameters[iTraj].momentum().x());
        m_py_ini.push_back(initialparameters[iTraj].momentum().y());
        m_pz_ini.push_back(initialparameters[iTraj].momentum().z());
        m_pT_ini.push_back(initialparameters[iTraj].transverseMomentum());
        m_eta_ini.push_back(eta(initialparameters[iTraj].position(geoContext)));

    // The trajectory entry indices and the multiTrajectory
    // const auto& [trackTips, mj] = traj.multiTrajectory();
    const auto& mj = traj.multiTrajectory();
    const auto& trackTips = traj.tips();

    if (trackTips.empty()) {
      ATH_MSG_WARNING("Empty multiTrajectory.");
      m_nMeasurements = -1;
      continue;
    }

    // Get the entry index for the single trajectory
    auto& trackTip = trackTips.front();

    // Collect the trajectory summary info
    auto trajState =
        Acts::MultiTrajectoryHelpers::trajectoryState(mj, trackTip);

    m_nMeasurements = trajState.nMeasurements;
//    if(m_nMeasurements<2)continue;
    m_nStates = trajState.nStates;
    m_nOutliers = trajState.nOutliers;
    m_nHoles = trajState.nHoles;
    m_chi2_fit = trajState.chi2Sum;
    m_ndf_fit = trajState.NDF;

    /// If it has track parameters, fill the values
    if (traj.hasTrackParameters(trackTip))
    {
      m_hasFittedParams = true;
      const auto &boundParam = traj.trackParameters(trackTip);
      const auto &parameter = boundParam.parameters();
      const auto &covariance = *boundParam.covariance();
      m_charge_fit = boundParam.charge();
      m_eLOC0_fit = parameter[Acts::eBoundLoc0];
      m_eLOC1_fit = parameter[Acts::eBoundLoc1];
      m_ePHI_fit = parameter[Acts::eBoundPhi];
      m_eTHETA_fit = parameter[Acts::eBoundTheta];
      m_eQOP_fit = parameter[Acts::eBoundQOverP];
      m_eT_fit = parameter[Acts::eBoundTime];
      m_err_eLOC0_fit =
          sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0));
      m_err_eLOC1_fit =
          sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1));
      m_err_ePHI_fit = sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi));
      m_err_eTHETA_fit =
          sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta));
      m_err_eQOP_fit = sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP));
      m_err_eT_fit = sqrt(covariance(Acts::eBoundTime, Acts::eBoundTime));

      m_px_fit = boundParam.momentum()(0);
      m_py_fit = boundParam.momentum()(1);
      m_pz_fit = boundParam.momentum()(2);
      m_x_fit  = boundParam.position(geoContext)(0);
      m_y_fit  = boundParam.position(geoContext)(1);
      m_z_fit  = boundParam.position(geoContext)(2);
    }

    m_nPredicted = 0;
    m_nFiltered = 0;
    m_nSmoothed = 0;

    mj.visitBackwards(trackTip, [&](const auto &state) {
      /// Only fill the track states with non-outlier measurement
      auto typeFlags = state.typeFlags();
      if (not typeFlags.test(Acts::TrackStateFlag::MeasurementFlag))
      {
        return true;
      }

      const auto& surface = state.referenceSurface();

      /// Get the geometry ID
      auto geoID = state.referenceSurface().geometryId();
      m_volumeID.push_back(geoID.volume());
      m_layerID.push_back(geoID.layer());
      m_moduleID.push_back(geoID.sensitive());

      // expand the local measurements into the full bound space
      Acts::BoundVector meas =
          state.projector().transpose() * state.calibrated();

      // extract local and global position
      Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
      const Acts::Vector3 dir = Acts::makeDirectionUnitFromPhiTheta(meas[Acts::eBoundPhi], meas[Acts::eBoundTheta]);
      Acts::Vector3 mom(1, 1, 1);
      Acts::Vector3 global =
          surface.localToGlobal(geoContext, local, dir);

      m_meas_eLOC0.push_back(state.calibrated()[Acts::eBoundLoc0]);
      m_meas_eLOC1.push_back(state.calibrated()[Acts::eBoundLoc1]);
      m_meas_cov_eLOC0.push_back(state.calibratedCovariance()(Acts::eBoundLoc0, Acts::eBoundLoc0));
      m_meas_cov_eLOC1.push_back(state.calibratedCovariance()(Acts::eBoundLoc1, Acts::eBoundLoc1));

      // fill the measurement info
      m_lx_hit.push_back(local[Acts::ePos0]);
      m_ly_hit.push_back(local[Acts::ePos1]);
      m_x_hit.push_back(global[Acts::ePos0]);
      m_y_hit.push_back(global[Acts::ePos1]);
      m_z_hit.push_back(global[Acts::ePos2]);

      /// Get the predicted parameter for this state
      bool predicted = false;
      if (state.hasPredicted())
      {
        predicted = true;
        m_nPredicted++;
        Acts::BoundTrackParameters parameter(
            state.referenceSurface().getSharedPtr(),
            state.predicted(),
            state.predictedCovariance());
        auto covariance = state.predictedCovariance();

        /// Local hit residual info
        auto H = state.effectiveProjector();
        auto resCov = state.effectiveCalibratedCovariance() +
                      H * covariance * H.transpose();
        auto residual = state.effectiveCalibrated() - H * state.predicted();

        /// Predicted residual
        m_res_eLOC0_prt.push_back(residual(Acts::eBoundLoc0));
//        m_res_eLOC1_prt.push_back(residual(Acts::eBoundLoc1));

        /// Predicted parameter pulls
        m_pull_eLOC0_prt.push_back(
            residual(Acts::eBoundLoc0) /
            sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));
//        m_pull_eLOC1_prt.push_back(
//            residual(Acts::eBoundLoc1) /
//            sqrt(resCov(Acts::eBoundLoc1, Acts::eBoundLoc1)));

        /// Predicted parameter
        m_eLOC0_prt.push_back(parameter.parameters()[Acts::eBoundLoc0]);
        m_eLOC1_prt.push_back(parameter.parameters()[Acts::eBoundLoc1]);
        m_ePHI_prt.push_back(parameter.parameters()[Acts::eBoundPhi]);
        m_eTHETA_prt.push_back(parameter.parameters()[Acts::eBoundTheta]);
        m_eQOP_prt.push_back(parameter.parameters()[Acts::eBoundQOverP]);
        m_eT_prt.push_back(parameter.parameters()[Acts::eBoundTime]);

        /// Predicted parameter Uncertainties
        m_err_eLOC0_prt.push_back(
            sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
        m_err_eLOC1_prt.push_back(
            sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1)));
        m_err_ePHI_prt.push_back(
            sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi)));
        m_err_eTHETA_prt.push_back(
            sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta)));
        m_err_eQOP_prt.push_back(
            sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP)));
        m_err_eT_prt.push_back(
            sqrt(covariance(Acts::eBoundTime, Acts::eBoundTime)));

        m_x_prt.push_back(parameter.position(geoContext).x());
        m_y_prt.push_back(parameter.position(geoContext).y());
        m_z_prt.push_back(parameter.position(geoContext).z());
        m_px_prt.push_back(parameter.momentum().x());
        m_py_prt.push_back(parameter.momentum().y());
        m_pz_prt.push_back(parameter.momentum().z());
        m_pT_prt.push_back(parameter.transverseMomentum());
        m_eta_prt.push_back(eta(parameter.position(geoContext)));
      }
      else
      {
        /// Push bad values if no predicted parameter
        m_eLOC0_prt.push_back(-9999);
        m_eLOC1_prt.push_back(-9999);
        m_ePHI_prt.push_back(-9999);
        m_eTHETA_prt.push_back(-9999);
        m_eQOP_prt.push_back(-9999);
        m_eT_prt.push_back(-9999);
        m_res_eLOC0_prt.push_back(-9999);
        m_res_eLOC1_prt.push_back(-9999);
        m_err_eLOC0_prt.push_back(-9999);
        m_err_eLOC1_prt.push_back(-9999);
        m_err_ePHI_prt.push_back(-9999);
        m_err_eTHETA_prt.push_back(-9999);
        m_err_eQOP_prt.push_back(-9999);
        m_err_eT_prt.push_back(-9999);
        m_pull_eLOC0_prt.push_back(-9999);
        m_pull_eLOC1_prt.push_back(-9999);
        m_x_prt.push_back(-9999);
        m_y_prt.push_back(-9999);
        m_z_prt.push_back(-9999);
        m_px_prt.push_back(-9999);
        m_py_prt.push_back(-9999);
        m_pz_prt.push_back(-9999);
        m_pT_prt.push_back(-9999);
        m_eta_prt.push_back(-9999);
      }

      bool filtered = false;
      if (state.hasFiltered())
      {
        filtered = true;
        m_nFiltered++;
        Acts::BoundTrackParameters parameter(
            state.referenceSurface().getSharedPtr(),
            state.filtered(),
            state.filteredCovariance());
        auto covariance = state.filteredCovariance();

        /// Local hit residual info
        auto H = state.effectiveProjector();
        auto resCov = state.effectiveCalibratedCovariance() +
                      H * covariance * H.transpose();
        auto residual = state.effectiveCalibrated() - H * state.filtered();

        /// Filtered residual
        m_res_eLOC0_flt.push_back(residual(Acts::eBoundLoc0));
//        m_res_eLOC1_flt.push_back(residual(Acts::eBoundLoc1));

        /// Filtered parameter pulls
        m_pull_eLOC0_flt.push_back(
            residual(Acts::eBoundLoc0) /
            sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));
//        m_pull_eLOC1_flt.push_back(
//            residual(Acts::eBoundLoc1) /
//            sqrt(resCov(Acts::eBoundLoc1, Acts::eBoundLoc1)));

        /// Filtered parameter
        m_eLOC0_flt.push_back(parameter.parameters()[Acts::eBoundLoc0]);
        m_eLOC1_flt.push_back(parameter.parameters()[Acts::eBoundLoc1]);
        m_ePHI_flt.push_back(parameter.parameters()[Acts::eBoundPhi]);
        m_eTHETA_flt.push_back(parameter.parameters()[Acts::eBoundTheta]);
        m_eQOP_flt.push_back(parameter.parameters()[Acts::eBoundQOverP]);
        m_eT_flt.push_back(parameter.parameters()[Acts::eBoundTime]);

        /// Filtered parameter uncertainties
        m_err_eLOC0_flt.push_back(
            sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
        m_err_eLOC1_flt.push_back(
            sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1)));
        m_err_ePHI_flt.push_back(
            sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi)));
        m_err_eTHETA_flt.push_back(
            sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta)));
        m_err_eQOP_flt.push_back(
            sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP)));
        m_err_eT_flt.push_back(
            sqrt(covariance(Acts::eBoundTime, Acts::eBoundTime)));

        /// Other filtered parameter info
        m_x_flt.push_back(parameter.position(geoContext).x());
        m_y_flt.push_back(parameter.position(geoContext).y());
        m_z_flt.push_back(parameter.position(geoContext).z());
        m_px_flt.push_back(parameter.momentum().x());
        m_py_flt.push_back(parameter.momentum().y());
        m_pz_flt.push_back(parameter.momentum().z());
        m_pT_flt.push_back(parameter.transverseMomentum());
        m_eta_flt.push_back(eta(parameter.position(geoContext)));
        m_chi2.push_back(state.chi2());

      }
      else
      {
        /// Push bad values if no filtered parameter
        m_eLOC0_flt.push_back(-9999);
        m_eLOC1_flt.push_back(-9999);
        m_ePHI_flt.push_back(-9999);
        m_eTHETA_flt.push_back(-9999);
        m_eQOP_flt.push_back(-9999);
        m_eT_flt.push_back(-9999);
        m_res_eLOC0_flt.push_back(-9999);
        m_res_eLOC1_flt.push_back(-9999);
        m_err_eLOC0_flt.push_back(-9999);
        m_err_eLOC1_flt.push_back(-9999);
        m_err_ePHI_flt.push_back(-9999);
        m_err_eTHETA_flt.push_back(-9999);
        m_err_eQOP_flt.push_back(-9999);
        m_err_eT_flt.push_back(-9999);
        m_pull_eLOC0_flt.push_back(-9999);
        m_pull_eLOC1_flt.push_back(-9999);
        m_x_flt.push_back(-9999);
        m_y_flt.push_back(-9999);
        m_z_flt.push_back(-9999);
        m_py_flt.push_back(-9999);
        m_pz_flt.push_back(-9999);
        m_pT_flt.push_back(-9999);
        m_eta_flt.push_back(-9999);
        m_chi2.push_back(-9999);
      }

      bool smoothed = false;
      if (state.hasSmoothed())
      {
        smoothed = true;
        m_nSmoothed++;
        Acts::BoundTrackParameters parameter(
            state.referenceSurface().getSharedPtr(),
            state.smoothed(),
            state.smoothedCovariance());
        auto covariance = state.smoothedCovariance();

        /// Local hit residual info
        auto H = state.effectiveProjector();
        auto resCov = state.effectiveCalibratedCovariance() +
                      H * covariance * H.transpose();
        auto residual = state.effectiveCalibrated() - H * state.smoothed();

        m_res_x_hit.push_back(residual(Acts::eBoundLoc0));
//        m_res_y_hit.push_back(residual(Acts::eBoundLoc1));
        m_err_x_hit.push_back(
            sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));
//        m_err_y_hit.push_back(
//            sqrt(resCov(Acts::eBoundLoc1, Acts::eBoundLoc1)));
        m_pull_x_hit.push_back(
            residual(Acts::eBoundLoc0) /
            sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));
//        m_pull_y_hit.push_back(
//            residual(Acts::eBoundLoc1) /
//            sqrt(resCov(Acts::eBoundLoc1, Acts::eBoundLoc1)));
        m_dim_hit.push_back(state.calibratedSize());

        /// Smoothed residual
        m_res_eLOC0_smt.push_back(residual(Acts::eBoundLoc0));
//        m_res_eLOC1_smt.push_back(residual(Acts::eBoundLoc1));

        /// Smoothed parameter pulls
        m_pull_eLOC0_smt.push_back(
            residual(Acts::eBoundLoc0) /
            sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));
//        m_pull_eLOC1_smt.push_back(
//            residual(Acts::eBoundLoc1) /
//            sqrt(resCov(Acts::eBoundLoc1, Acts::eBoundLoc1)));

        /// Smoothed parameter
        m_eLOC0_smt.push_back(parameter.parameters()[Acts::eBoundLoc0]);
        m_eLOC1_smt.push_back(parameter.parameters()[Acts::eBoundLoc1]);
        m_ePHI_smt.push_back(parameter.parameters()[Acts::eBoundPhi]);
        m_eTHETA_smt.push_back(parameter.parameters()[Acts::eBoundTheta]);
        m_eQOP_smt.push_back(parameter.parameters()[Acts::eBoundQOverP]);
        m_eT_smt.push_back(parameter.parameters()[Acts::eBoundTime]);

        /// Smoothed parameter uncertainties
        m_err_eLOC0_smt.push_back(
            sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
        m_err_eLOC1_smt.push_back(
            sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1)));
        m_err_ePHI_smt.push_back(
            sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi)));
        m_err_eTHETA_smt.push_back(
            sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta)));
        m_err_eQOP_smt.push_back(
            sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP)));
        m_err_eT_smt.push_back(
            sqrt(covariance(Acts::eBoundTime, Acts::eBoundTime)));

        m_x_smt.push_back(parameter.position(geoContext).x());
        m_y_smt.push_back(parameter.position(geoContext).y());
        m_z_smt.push_back(parameter.position(geoContext).z());
        m_px_smt.push_back(parameter.momentum().x());
        m_py_smt.push_back(parameter.momentum().y());
        m_pz_smt.push_back(parameter.momentum().z());
        m_pT_smt.push_back(parameter.transverseMomentum());
        m_eta_smt.push_back(eta(parameter.position(geoContext)));
      }
      else
      {
        /// Push bad values if no smoothed parameter
        m_eLOC0_smt.push_back(-9999);
        m_eLOC1_smt.push_back(-9999);
        m_ePHI_smt.push_back(-9999);
        m_eTHETA_smt.push_back(-9999);
        m_eQOP_smt.push_back(-9999);
        m_eT_smt.push_back(-9999);
        m_res_eLOC0_smt.push_back(-9999);
        m_res_eLOC1_smt.push_back(-9999);
        m_err_eLOC0_smt.push_back(-9999);
        m_err_eLOC1_smt.push_back(-9999);
        m_err_ePHI_smt.push_back(-9999);
        m_err_eTHETA_smt.push_back(-9999);
        m_err_eQOP_smt.push_back(-9999);
        m_err_eT_smt.push_back(-9999);
        m_pull_eLOC0_smt.push_back(-9999);
        m_pull_eLOC1_smt.push_back(-9999);
        m_x_smt.push_back(-9999);
        m_y_smt.push_back(-9999);
        m_z_smt.push_back(-9999);
        m_px_smt.push_back(-9999);
        m_py_smt.push_back(-9999);
        m_pz_smt.push_back(-9999);
        m_pT_smt.push_back(-9999);
        m_eta_smt.push_back(-9999);
        m_res_x_hit.push_back(-9999);
        m_res_y_hit.push_back(-9999);
        m_err_x_hit.push_back(-9999);
        m_err_y_hit.push_back(-9999);
        m_pull_x_hit.push_back(-9999);
        m_pull_y_hit.push_back(-9999);
        m_dim_hit.push_back(-9999);
      }

      /// Save whether or not states had various KF steps
      m_prt.push_back(predicted);
      m_flt.push_back(filtered);
      m_smt.push_back(smoothed);

      return true;
    } // Finish lambda function
    ); // Finish multi trajectory visitBackwards call

    iTraj++;
  }
  m_tree->Fill();

  clearVariables();
}


void TrajectoryWriterTool::clearVariables() const {
  m_volumeID.clear();
  m_layerID.clear();
  m_moduleID.clear();
  m_lx_hit.clear();
  m_ly_hit.clear();
  m_x_hit.clear();
  m_y_hit.clear();
  m_z_hit.clear();
  m_meas_eLOC0.clear();
  m_meas_eLOC1.clear();
  m_meas_cov_eLOC0.clear();
  m_meas_cov_eLOC1.clear();
  m_res_x_hit.clear();
  m_res_y_hit.clear();
  m_err_x_hit.clear();
  m_err_y_hit.clear();
  m_pull_x_hit.clear();
  m_pull_y_hit.clear();
  m_dim_hit.clear();

  m_prt.clear();
  m_eLOC0_prt.clear();
  m_eLOC1_prt.clear();
  m_ePHI_prt.clear();
  m_eTHETA_prt.clear();
  m_eQOP_prt.clear();
  m_eT_prt.clear();
  m_res_eLOC0_prt.clear();
  m_res_eLOC1_prt.clear();
  m_err_eLOC0_prt.clear();
  m_err_eLOC1_prt.clear();
  m_err_ePHI_prt.clear();
  m_err_eTHETA_prt.clear();
  m_err_eQOP_prt.clear();
  m_err_eT_prt.clear();
  m_pull_eLOC0_prt.clear();
  m_pull_eLOC1_prt.clear();
  m_x_prt.clear();
  m_y_prt.clear();
  m_z_prt.clear();
  m_px_prt.clear();
  m_py_prt.clear();
  m_pz_prt.clear();
  m_eta_prt.clear();
  m_pT_prt.clear();

  m_flt.clear();
  m_eLOC0_flt.clear();
  m_eLOC1_flt.clear();
  m_ePHI_flt.clear();
  m_eTHETA_flt.clear();
  m_eQOP_flt.clear();
  m_eT_flt.clear();
  m_res_eLOC0_flt.clear();
  m_res_eLOC1_flt.clear();
  m_err_eLOC0_flt.clear();
  m_err_eLOC1_flt.clear();
  m_err_ePHI_flt.clear();
  m_err_eTHETA_flt.clear();
  m_err_eQOP_flt.clear();
  m_err_eT_flt.clear();
  m_pull_eLOC0_flt.clear();
  m_pull_eLOC1_flt.clear();
  m_x_flt.clear();
  m_y_flt.clear();
  m_z_flt.clear();
  m_px_flt.clear();
  m_py_flt.clear();
  m_pz_flt.clear();
  m_eta_flt.clear();
  m_pT_flt.clear();
  m_chi2.clear();

  m_smt.clear();
  m_eLOC0_smt.clear();
  m_eLOC1_smt.clear();
  m_ePHI_smt.clear();
  m_eTHETA_smt.clear();
  m_eQOP_smt.clear();
  m_eT_smt.clear();
  m_res_eLOC0_smt.clear();
  m_res_eLOC1_smt.clear();
  m_err_eLOC0_smt.clear();
  m_err_eLOC1_smt.clear();
  m_err_ePHI_smt.clear();
  m_err_eTHETA_smt.clear();
  m_err_eQOP_smt.clear();
  m_err_eT_smt.clear();
  m_pull_eLOC0_smt.clear();
  m_pull_eLOC1_smt.clear();
  m_x_smt.clear();
  m_y_smt.clear();
  m_z_smt.clear();
  m_px_smt.clear();
  m_py_smt.clear();
  m_pz_smt.clear();
  m_eta_smt.clear();
  m_pT_smt.clear();
  m_eLOC0_ini.clear();
  m_eLOC1_ini.clear();
  m_ePHI_ini.clear();
  m_eTHETA_ini.clear();
  m_eQOP_ini.clear();
  m_eT_ini.clear();
  m_err_eLOC0_ini.clear();
  m_err_eLOC1_ini.clear();
  m_err_ePHI_ini.clear();
  m_err_eTHETA_ini.clear();
  m_err_eQOP_ini.clear();
  m_err_eT_ini.clear();
  m_x_ini.clear();
  m_y_ini.clear();
  m_z_ini.clear();
  m_px_ini.clear();
  m_py_ini.clear();
  m_pz_ini.clear();
  m_eta_ini.clear();
  m_pT_ini.clear();

}
