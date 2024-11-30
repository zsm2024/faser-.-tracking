// Amg

// Ensure Eigen plugin comes first
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "RootTrajectoryStatesWriterTool.h"

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/MultiTrajectoryHelpers.hpp"
#include "Acts/EventData/detail/TransformationBoundToFree.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "FaserActsRecMultiTrajectory.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackClassification.h"
#include <TFile.h>
#include <TTree.h>

//constexpr float nan = std::numeric_limits<float>::quiet_NaN();

using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::perp;
using Acts::VectorHelpers::phi;
using Acts::VectorHelpers::theta;

RootTrajectoryStatesWriterTool::RootTrajectoryStatesWriterTool(
    const std::string& type, const std::string& name, const IInterface* parent)
    : AthAlgTool(type, name, parent) {}

StatusCode RootTrajectoryStatesWriterTool::initialize() {
  ATH_CHECK(m_mcEventCollectionKey.initialize());
  ATH_CHECK(m_simDataCollectionKey.initialize());
  ATH_CHECK(m_faserSiHitKey.initialize());
  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));
  ATH_CHECK(detStore()->retrieve(m_detMgr, "SCT"));

  if (!m_noDiagnostics) {
    std::string filePath = m_filePath;
    std::string treeName = m_treeName;
    m_outputFile = TFile::Open(filePath.c_str(), "RECREATE");
    if (m_outputFile == nullptr) {
      ATH_MSG_ERROR("Unable to open output file at " << m_filePath);
      return StatusCode::FAILURE;
    }
    m_outputFile->cd();
    m_outputTree = new TTree(treeName.c_str(), treeName.c_str());
    if (m_outputTree == nullptr) {
      ATH_MSG_ERROR("Unable to create TTree");
      return StatusCode::FAILURE;
    }

    m_outputTree = new TTree("tree", "tree");

    m_outputTree->Branch("event_nr", &m_eventNr);
    m_outputTree->Branch("track_nr", &m_trackNr);

    m_outputTree->Branch("t_x", &m_t_x);
    m_outputTree->Branch("t_y", &m_t_y);
    m_outputTree->Branch("t_z", &m_t_z);
    m_outputTree->Branch("t_dx", &m_t_dx);
    m_outputTree->Branch("t_dy", &m_t_dy);
    m_outputTree->Branch("t_dz", &m_t_dz);
    m_outputTree->Branch("t_eLOC0", &m_t_eLOC0);
    m_outputTree->Branch("t_eLOC1", &m_t_eLOC1);
    m_outputTree->Branch("t_ePHI", &m_t_ePHI);
    m_outputTree->Branch("t_eTHETA", &m_t_eTHETA);
    m_outputTree->Branch("t_eQOP", &m_t_eQOP);
    m_outputTree->Branch("t_eT", &m_t_eT);
    m_outputTree->Branch("t_rMax", &m_t_rMax);

    m_outputTree->Branch("nStates", &m_nStates);
    m_outputTree->Branch("nMeasurements", &m_nMeasurements);
    m_outputTree->Branch("nOutliers", &m_nOutliers);
    m_outputTree->Branch("nHoles", &m_nHoles);
    m_outputTree->Branch("chi2Sum", &m_chi2Sum);
    m_outputTree->Branch("NDF", &m_NDF);
    m_outputTree->Branch("chi2", &m_chi2);

    m_outputTree->Branch("volume_id", &m_volumeID);
    m_outputTree->Branch("layer_id", &m_layerID);
    m_outputTree->Branch("module_id", &m_moduleID);
    m_outputTree->Branch("station", &m_station);
    m_outputTree->Branch("layer", &m_layer);
    m_outputTree->Branch("phi_module", &m_phi_module);
    m_outputTree->Branch("eta_module", &m_eta_module);
    m_outputTree->Branch("side", &m_side);
    m_outputTree->Branch("pathLength", &m_pathLength);
    m_outputTree->Branch("l_x_hit", &m_lx_hit);
    m_outputTree->Branch("l_y_hit", &m_ly_hit);
    m_outputTree->Branch("g_x_hit", &m_x_hit);
    m_outputTree->Branch("g_y_hit", &m_y_hit);
    m_outputTree->Branch("g_z_hit", &m_z_hit);
    m_outputTree->Branch("res_x_hit", &m_res_x_hit);
    m_outputTree->Branch("res_y_hit", &m_res_y_hit);
    m_outputTree->Branch("err_x_hit", &m_err_x_hit);
    m_outputTree->Branch("err_y_hit", &m_err_y_hit);
    m_outputTree->Branch("pull_x_hit", &m_pull_x_hit);
    m_outputTree->Branch("pull_y_hit", &m_pull_y_hit);
    m_outputTree->Branch("dim_hit", &m_dim_hit);
    m_outputTree->Branch("rMax_hit", &m_rMax_hit);
    m_outputTree->Branch("index_hit", &m_index_hit);


    m_outputTree->Branch("nPredicted", &m_nParams[ePredicted]);
    m_outputTree->Branch("predicted", &m_hasParams[ePredicted]);
    m_outputTree->Branch("eLOC0_prt", &m_eLOC0[ePredicted]);
    m_outputTree->Branch("eLOC1_prt", &m_eLOC1[ePredicted]);
    m_outputTree->Branch("ePHI_prt", &m_ePHI[ePredicted]);
    m_outputTree->Branch("eTHETA_prt", &m_eTHETA[ePredicted]);
    m_outputTree->Branch("eQOP_prt", &m_eQOP[ePredicted]);
    m_outputTree->Branch("eT_prt", &m_eT[ePredicted]);
    m_outputTree->Branch("res_eLOC0_prt", &m_res_eLOC0[ePredicted]);
    m_outputTree->Branch("res_eLOC1_prt", &m_res_eLOC1[ePredicted]);
    m_outputTree->Branch("res_ePHI_prt", &m_res_ePHI[ePredicted]);
    m_outputTree->Branch("res_eTHETA_prt", &m_res_eTHETA[ePredicted]);
    m_outputTree->Branch("res_eQOP_prt", &m_res_eQOP[ePredicted]);
    m_outputTree->Branch("res_eT_prt", &m_res_eT[ePredicted]);
    m_outputTree->Branch("err_eLOC0_prt", &m_err_eLOC0[ePredicted]);
    m_outputTree->Branch("err_eLOC1_prt", &m_err_eLOC1[ePredicted]);
    m_outputTree->Branch("err_ePHI_prt", &m_err_ePHI[ePredicted]);
    m_outputTree->Branch("err_eTHETA_prt", &m_err_eTHETA[ePredicted]);
    m_outputTree->Branch("err_eQOP_prt", &m_err_eQOP[ePredicted]);
    m_outputTree->Branch("err_eT_prt", &m_err_eT[ePredicted]);
    m_outputTree->Branch("pull_eLOC0_prt", &m_pull_eLOC0[ePredicted]);
    m_outputTree->Branch("pull_eLOC1_prt", &m_pull_eLOC1[ePredicted]);
    m_outputTree->Branch("pull_ePHI_prt", &m_pull_ePHI[ePredicted]);
    m_outputTree->Branch("pull_eTHETA_prt", &m_pull_eTHETA[ePredicted]);
    m_outputTree->Branch("pull_eQOP_prt", &m_pull_eQOP[ePredicted]);
    m_outputTree->Branch("pull_eT_prt", &m_pull_eT[ePredicted]);
    m_outputTree->Branch("g_x_prt", &m_x[ePredicted]);
    m_outputTree->Branch("g_y_prt", &m_y[ePredicted]);
    m_outputTree->Branch("g_z_prt", &m_z[ePredicted]);
    m_outputTree->Branch("px_prt", &m_px[ePredicted]);
    m_outputTree->Branch("py_prt", &m_py[ePredicted]);
    m_outputTree->Branch("pz_prt", &m_pz[ePredicted]);
    m_outputTree->Branch("eta_prt", &m_eta[ePredicted]);
    m_outputTree->Branch("pT_prt", &m_pT[ePredicted]);
    m_outputTree->Branch("rMax_prt", &m_rMax[ePredicted]);


    m_outputTree->Branch("nFiltered", &m_nParams[eFiltered]);
    m_outputTree->Branch("filtered", &m_hasParams[eFiltered]);
    m_outputTree->Branch("eLOC0_flt", &m_eLOC0[eFiltered]);
    m_outputTree->Branch("eLOC1_flt", &m_eLOC1[eFiltered]);
    m_outputTree->Branch("ePHI_flt", &m_ePHI[eFiltered]);
    m_outputTree->Branch("eTHETA_flt", &m_eTHETA[eFiltered]);
    m_outputTree->Branch("eQOP_flt", &m_eQOP[eFiltered]);
    m_outputTree->Branch("eT_flt", &m_eT[eFiltered]);
    m_outputTree->Branch("res_eLOC0_flt", &m_res_eLOC0[eFiltered]);
    m_outputTree->Branch("res_eLOC1_flt", &m_res_eLOC1[eFiltered]);
    m_outputTree->Branch("res_ePHI_flt", &m_res_ePHI[eFiltered]);
    m_outputTree->Branch("res_eTHETA_flt", &m_res_eTHETA[eFiltered]);
    m_outputTree->Branch("res_eQOP_flt", &m_res_eQOP[eFiltered]);
    m_outputTree->Branch("res_eT_flt", &m_res_eT[eFiltered]);
    m_outputTree->Branch("err_eLOC0_flt", &m_err_eLOC0[eFiltered]);
    m_outputTree->Branch("err_eLOC1_flt", &m_err_eLOC1[eFiltered]);
    m_outputTree->Branch("err_ePHI_flt", &m_err_ePHI[eFiltered]);
    m_outputTree->Branch("err_eTHETA_flt", &m_err_eTHETA[eFiltered]);
    m_outputTree->Branch("err_eQOP_flt", &m_err_eQOP[eFiltered]);
    m_outputTree->Branch("err_eT_flt", &m_err_eT[eFiltered]);
    m_outputTree->Branch("pull_eLOC0_flt", &m_pull_eLOC0[eFiltered]);
    m_outputTree->Branch("pull_eLOC1_flt", &m_pull_eLOC1[eFiltered]);
    m_outputTree->Branch("pull_ePHI_flt", &m_pull_ePHI[eFiltered]);
    m_outputTree->Branch("pull_eTHETA_flt", &m_pull_eTHETA[eFiltered]);
    m_outputTree->Branch("pull_eQOP_flt", &m_pull_eQOP[eFiltered]);
    m_outputTree->Branch("pull_eT_flt", &m_pull_eT[eFiltered]);
    m_outputTree->Branch("g_x_flt", &m_x[eFiltered]);
    m_outputTree->Branch("g_y_flt", &m_y[eFiltered]);
    m_outputTree->Branch("g_z_flt", &m_z[eFiltered]);
    m_outputTree->Branch("px_flt", &m_px[eFiltered]);
    m_outputTree->Branch("py_flt", &m_py[eFiltered]);
    m_outputTree->Branch("pz_flt", &m_pz[eFiltered]);
    m_outputTree->Branch("eta_flt", &m_eta[eFiltered]);
    m_outputTree->Branch("pT_flt", &m_pT[eFiltered]);
    m_outputTree->Branch("rMax_flt", &m_rMax[eFiltered]);

    m_outputTree->Branch("nSmoothed", &m_nParams[eSmoothed]);
    m_outputTree->Branch("smoothed", &m_hasParams[eSmoothed]);
    m_outputTree->Branch("eLOC0_smt", &m_eLOC0[eSmoothed]);
    m_outputTree->Branch("eLOC1_smt", &m_eLOC1[eSmoothed]);
    m_outputTree->Branch("ePHI_smt", &m_ePHI[eSmoothed]);
    m_outputTree->Branch("eTHETA_smt", &m_eTHETA[eSmoothed]);
    m_outputTree->Branch("eQOP_smt", &m_eQOP[eSmoothed]);
    m_outputTree->Branch("eT_smt", &m_eT[eSmoothed]);
    m_outputTree->Branch("res_eLOC0_smt", &m_res_eLOC0[eSmoothed]);
    m_outputTree->Branch("res_eLOC1_smt", &m_res_eLOC1[eSmoothed]);
    m_outputTree->Branch("res_ePHI_smt", &m_res_ePHI[eSmoothed]);
    m_outputTree->Branch("res_eTHETA_smt", &m_res_eTHETA[eSmoothed]);
    m_outputTree->Branch("res_eQOP_smt", &m_res_eQOP[eSmoothed]);
    m_outputTree->Branch("res_eT_smt", &m_res_eT[eSmoothed]);
    m_outputTree->Branch("err_eLOC0_smt", &m_err_eLOC0[eSmoothed]);
    m_outputTree->Branch("err_eLOC1_smt", &m_err_eLOC1[eSmoothed]);
    m_outputTree->Branch("err_ePHI_smt", &m_err_ePHI[eSmoothed]);
    m_outputTree->Branch("err_eTHETA_smt", &m_err_eTHETA[eSmoothed]);
    m_outputTree->Branch("err_eQOP_smt", &m_err_eQOP[eSmoothed]);
    m_outputTree->Branch("err_eT_smt", &m_err_eT[eSmoothed]);
    m_outputTree->Branch("pull_eLOC0_smt", &m_pull_eLOC0[eSmoothed]);
    m_outputTree->Branch("pull_eLOC1_smt", &m_pull_eLOC1[eSmoothed]);
    m_outputTree->Branch("pull_ePHI_smt", &m_pull_ePHI[eSmoothed]);
    m_outputTree->Branch("pull_eTHETA_smt", &m_pull_eTHETA[eSmoothed]);
    m_outputTree->Branch("pull_eQOP_smt", &m_pull_eQOP[eSmoothed]);
    m_outputTree->Branch("pull_eT_smt", &m_pull_eT[eSmoothed]);
    m_outputTree->Branch("g_x_smt", &m_x[eSmoothed]);
    m_outputTree->Branch("g_y_smt", &m_y[eSmoothed]);
    m_outputTree->Branch("g_z_smt", &m_z[eSmoothed]);
    m_outputTree->Branch("px_smt", &m_px[eSmoothed]);
    m_outputTree->Branch("py_smt", &m_py[eSmoothed]);
    m_outputTree->Branch("pz_smt", &m_pz[eSmoothed]);
    m_outputTree->Branch("eta_smt", &m_eta[eSmoothed]);
    m_outputTree->Branch("pT_smt", &m_pT[eSmoothed]);
    m_outputTree->Branch("rMax_smt", &m_rMax[eSmoothed]);


    m_outputTree->Branch("nUnbiased", &m_nParams[eUnbiased]);
    m_outputTree->Branch("unbiased", &m_hasParams[eUnbiased]);
    m_outputTree->Branch("eLOC0_ubs", &m_eLOC0[eUnbiased]);
    m_outputTree->Branch("eLOC1_ubs", &m_eLOC1[eUnbiased]);
    m_outputTree->Branch("ePHI_ubs", &m_ePHI[eUnbiased]);
    m_outputTree->Branch("eTHETA_ubs", &m_eTHETA[eUnbiased]);
    m_outputTree->Branch("eQOP_ubs", &m_eQOP[eUnbiased]);
    m_outputTree->Branch("eT_ubs", &m_eT[eUnbiased]);
    m_outputTree->Branch("res_eLOC0_ubs", &m_res_eLOC0[eUnbiased]);
    m_outputTree->Branch("res_eLOC1_ubs", &m_res_eLOC1[eUnbiased]);
    m_outputTree->Branch("res_ePHI_ubs", &m_res_ePHI[eUnbiased]);
    m_outputTree->Branch("res_eTHETA_ubs", &m_res_eTHETA[eUnbiased]);
    m_outputTree->Branch("res_eQOP_ubs", &m_res_eQOP[eUnbiased]);
    m_outputTree->Branch("res_eT_ubs", &m_res_eT[eUnbiased]);
    m_outputTree->Branch("err_eLOC0_ubs", &m_err_eLOC0[eUnbiased]);
    m_outputTree->Branch("err_eLOC1_ubs", &m_err_eLOC1[eUnbiased]);
    m_outputTree->Branch("err_ePHI_ubs", &m_err_ePHI[eUnbiased]);
    m_outputTree->Branch("err_eTHETA_ubs", &m_err_eTHETA[eUnbiased]);
    m_outputTree->Branch("err_eQOP_ubs", &m_err_eQOP[eUnbiased]);
    m_outputTree->Branch("err_eT_ubs", &m_err_eT[eUnbiased]);
    m_outputTree->Branch("pull_eLOC0_ubs", &m_pull_eLOC0[eUnbiased]);
    m_outputTree->Branch("pull_eLOC1_ubs", &m_pull_eLOC1[eUnbiased]);
    m_outputTree->Branch("pull_ePHI_ubs", &m_pull_ePHI[eUnbiased]);
    m_outputTree->Branch("pull_eTHETA_ubs", &m_pull_eTHETA[eUnbiased]);
    m_outputTree->Branch("pull_eQOP_ubs", &m_pull_eQOP[eUnbiased]);
    m_outputTree->Branch("pull_eT_ubs", &m_pull_eT[eUnbiased]);
    m_outputTree->Branch("g_x_ubs", &m_x[eUnbiased]);
    m_outputTree->Branch("g_y_ubs", &m_y[eUnbiased]);
    m_outputTree->Branch("g_z_ubs", &m_z[eUnbiased]);
    m_outputTree->Branch("px_ubs", &m_px[eUnbiased]);
    m_outputTree->Branch("py_ubs", &m_py[eUnbiased]);
    m_outputTree->Branch("pz_ubs", &m_pz[eUnbiased]);
    m_outputTree->Branch("eta_ubs", &m_eta[eUnbiased]);
    m_outputTree->Branch("pT_ubs", &m_pT[eUnbiased]);
    m_outputTree->Branch("rMax_ubs", &m_rMax[eUnbiased]);
    


    m_outputTree->Branch("chi2", &m_chi2);
  }

  return StatusCode::SUCCESS;
}


StatusCode RootTrajectoryStatesWriterTool::finalize() {
  if (!m_noDiagnostics) {
    m_outputFile->cd();
    m_outputTree->Write();
    m_outputFile->Close();
  }
  return StatusCode::SUCCESS;
}

StatusCode RootTrajectoryStatesWriterTool::write(const Acts::GeometryContext& gctx, const FaserActsTrackContainer& tracks, bool isMC) const {
  float nan = std::numeric_limits<float>::quiet_NaN();

  if (m_outputFile == nullptr)
    return StatusCode::SUCCESS;

  // Get the event number
  const EventContext& ctx = Gaudi::Hive::currentContext();
  m_eventNr = ctx.eventID().event_number();

  std::map<int, const HepMC::GenParticle*> particles {};
  std::map<std::pair<int, Identifier>, const FaserSiHit*> siHitMap;

  std::shared_ptr<TrackerSimDataCollection> simData {nullptr};

  if (isMC) {
    SG::ReadHandle<McEventCollection> mcEvents {m_mcEventCollectionKey, ctx};
        ATH_CHECK(mcEvents.isValid());
    if (mcEvents->size() != 1) {
      ATH_MSG_ERROR("There should be exactly one event in the McEventCollection.");
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("Found " << mcEvents->front()->particles_size() << " particles.");
    for (const auto& particle : mcEvents->front()->particles()) {
       particles[HepMC::barcode(particle)] = &(*particle);
    }

    SG::ReadHandle<TrackerSimDataCollection> simDataHandle {m_simDataCollectionKey, ctx};
        ATH_CHECK(simDataHandle.isValid());
    simData = std::make_shared<TrackerSimDataCollection>(*simDataHandle);

    SG::ReadHandle<FaserSiHitCollection> siHitCollection {m_faserSiHitKey, ctx};
    ATH_CHECK(siHitCollection.isValid());
    for (const FaserSiHit& hit : *siHitCollection) {
      int trackNumber = hit.trackNumber();
      Identifier id = m_idHelper->wafer_id(hit.getStation(), hit.getPlane(), hit.getRow(), hit.getModule(), hit.getSensor());
      siHitMap[std::make_pair(trackNumber, id)] = &hit;
    }
  }

  std::vector<ParticleHitCount> particleHitCounts = {};
  for (const auto& track : tracks) {
    m_trackNr = track.index();

    // Collect the track summary info
    m_nMeasurements = track.nMeasurements();
    m_nStates = track.nTrackStates();
    m_nHoles = track.nHoles();
    m_nOutliers = track.nOutliers();
    m_chi2Sum = track.chi2();
    m_NDF = track.nDoF();

    // Get the majority truth particle to this track
    int barcode = std::numeric_limits<int>::quiet_NaN();
    // int truthQ = std::numeric_limits<int>::quiet_NaN();
    // float truthMomentum = nan;
    float truthLOC0 = nan;
    float truthLOC1 = nan;
    float truthPHI = nan;
    float truthTHETA = nan;
    float truthQOP = nan;
    float truthTIME = nan;

    if (isMC) {
      // truthQ = 1;
      // truthMomentum = 1;
      //@todo: this can break and the reasons needs to be understood 
      identifyContributingParticles(*simData, track, particleHitCounts);
      if (not particleHitCounts.empty()) {
        // Get the barcode of the majority truth particle
        barcode = particleHitCounts.front().particleId;
        // Find the truth particle via the barcode
        auto ip = particles.find(barcode);
        if (ip != particles.end()) {
          // const auto& particle = ip->second;
          ATH_MSG_DEBUG("Find the truth particle with barcode = " << barcode);
          // Get the truth particle charge
          // FIXME find better way to access charge of simulated particle, this does not work for
          // pions which have a positive pdg code (211) and positive charge
          // truthQ = particle->pdg_id() > 0 ? -1 : 1;
          // truthMomentum = particle->momentum().rho() * m_MeV2GeV;
        } else {
          ATH_MSG_WARNING("Truth particle with barcode = " << barcode << " not found!");
        }
      }
    }

    // Get the trackStates on the trajectory
    m_nParams = {0, 0, 0, 0};
    double t_rMax=0, rMax_hit = 0;
    std::array<double,3> rMax_fit={0, 0, 0};
 
    for (const auto& state : track.trackStatesReversed()) {
      const auto& surface = state.referenceSurface();
 
      // we only fill the track states with non-outlier measurement
      auto typeFlags = state.typeFlags();
      if (not typeFlags.test(Acts::TrackStateFlag::MeasurementFlag)) {
        continue;
      }

      // get the truth hits corresponding to this trackState
      IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
      const Tracker::FaserSCT_Cluster* cluster = sl.hit(); 
      Identifier id = cluster->identify();
      Identifier waferId = m_idHelper->wafer_id(id);
      // if (siHitMap.count(std::make_pair(barcode, waferId)) == 0) {
      //   ATH_MSG_WARNING("no FaserSiHit for hit with id " << id << " from particle " << barcode);
      //   return true;
      // }

      Acts::Vector3 truthUnitDir(1,1,1);
      if (isMC && siHitMap.count(std::make_pair(barcode, waferId)) != 0) {
        const FaserSiHit* siHit = siHitMap.find(std::make_pair(barcode, waferId))->second;
        HepGeom::Point3D localStartPos = siHit->localStartPosition();
        HepGeom::Point3D localEndPos = siHit->localEndPosition();
        HepGeom::Point3D<double> localPos = 0.5 * (localEndPos + localStartPos);
        auto truthLocal = Acts::Vector2(localPos.y(), localPos.z());
        const TrackerDD::SiDetectorElement* element = m_detMgr->getDetectorElement(id);
        const HepGeom::Point3D<double> globalStartPosition =
            Amg::EigenTransformToCLHEP(element->transformHit()) * localStartPos;
        const HepGeom::Point3D<double> globalEndPosition =
            Amg::EigenTransformToCLHEP(element->transformHit()) * localEndPos;
        auto globalPosition = 0.5 * (globalStartPosition + globalEndPosition);
        auto globalDirection = globalEndPosition - globalStartPosition;
        truthUnitDir = Acts::Vector3(globalDirection.x(), globalDirection.y(), globalDirection.z()).normalized();
        auto truthPos = Acts::Vector3(globalPosition.x() , globalPosition.y(), globalPosition.z());
        // FIXME get truthQOP for each state

        // fill the truth hit info
        m_t_x.push_back(truthPos[Acts::ePos0]);
        m_t_y.push_back(truthPos[Acts::ePos1]);
        m_t_z.push_back(truthPos[Acts::ePos2]);
        m_t_dx.push_back(truthUnitDir[Acts::eMom0]);
        m_t_dy.push_back(truthUnitDir[Acts::eMom1]);
        m_t_dz.push_back(truthUnitDir[Acts::eMom2]);
        auto t_r = std::hypot(truthPos[Acts::ePos0],truthPos[Acts::ePos1]);
        if(t_r> t_rMax){
          t_rMax = t_r;
        }

        // get the truth track parameter at this track State
        float truthLOC0 = truthLocal[Acts::ePos0];
        float truthLOC1 = truthLocal[Acts::ePos1];
        float truthTIME = siHit->meanTime();
        float truthPHI = phi(truthUnitDir);
        float truthTHETA = theta(truthUnitDir);

        // fill the truth track parameter at this track State
        m_t_eLOC0.push_back(truthLOC0);
        m_t_eLOC1.push_back(truthLOC1);
        m_t_ePHI.push_back(truthPHI);
        m_t_eTHETA.push_back(truthTHETA);
        m_t_eQOP.push_back(truthQOP);
        m_t_eT.push_back(truthTIME);
      } else {
        if(isMC){
          ATH_MSG_WARNING("Hit with particle barcode " << barcode  <<" and wafer Id " << waferId <<" not found.");
        }
        m_t_x.push_back(nan);
        m_t_y.push_back(nan);
        m_t_z.push_back(nan);
        m_t_dx.push_back(nan);
        m_t_dy.push_back(nan);
        m_t_dz.push_back(nan);
        m_t_eLOC0.push_back(nan);
        m_t_eLOC1.push_back(nan);
        m_t_ePHI.push_back(nan);
        m_t_eTHETA.push_back(nan);
        m_t_eQOP.push_back(nan);
        m_t_eT.push_back(nan);
      }

      // get the geometry ID
      auto geoID = surface.geometryId();
      m_volumeID.push_back(geoID.volume());
      m_layerID.push_back(geoID.layer());
      m_moduleID.push_back(geoID.sensitive());

      // get wafer information
      m_station.push_back(m_idHelper->station(id));
      m_layer.push_back(m_idHelper->layer(id));
      m_phi_module.push_back(m_idHelper->phi_module(id));
      m_eta_module.push_back(m_idHelper->eta_module(id));
      m_side.push_back(m_idHelper->side(id));

      // get the path length
      m_pathLength.push_back(state.pathLength());

      // fill the chi2
      m_chi2.push_back(state.chi2());

      // expand the local measurements into the full bound space
      Acts::BoundVector meas = state.effectiveProjector().transpose() *
                               state.effectiveCalibrated();
      // extract local and global position
      Acts::Vector2 local(meas[Acts::eBoundLoc0], meas[Acts::eBoundLoc1]);
      Acts::Vector3 global =
          surface.localToGlobal(gctx, local, truthUnitDir);

      // fill the measurement info
      m_lx_hit.push_back(local[Acts::ePos0]);
      m_ly_hit.push_back(local[Acts::ePos1]);
      m_x_hit.push_back(global[Acts::ePos0]);
      m_y_hit.push_back(global[Acts::ePos1]);
      m_z_hit.push_back(global[Acts::ePos2]);
      auto r_hit = std::hypot(global[Acts::ePos0],global[Acts::ePos1]);
      if(r_hit>rMax_hit){
        rMax_hit = r_hit;
      }

      // lambda to get the fitted track parameters
      auto getTrackParams = [&](unsigned int ipar)
          -> std::optional<std::pair<Acts::BoundVector, Acts::BoundMatrix>> {
        if (ipar == ePredicted && state.hasPredicted()) {
          return std::make_pair(state.predicted(), state.predictedCovariance());
        }
        if (ipar == eFiltered && state.hasFiltered()) {
          return std::make_pair(state.filtered(), state.filteredCovariance());
        }
        if (ipar == eSmoothed && state.hasSmoothed()) {
          return std::make_pair(state.smoothed(), state.smoothedCovariance());
        }
        if (ipar == eUnbiased && state.hasSmoothed() && state.hasProjector()) {
          // calculate the unbiased track parameters (i.e. fitted track
          // parameters with this measurement removed) using Eq.(12a)-Eq.(12c)
          // of NIMA 262, 444 (1987)
          auto m = state.effectiveCalibrated();
          auto H = state.effectiveProjector();
          auto V = state.effectiveCalibratedCovariance();
          auto K =
              (state.smoothedCovariance() * H.transpose() *
               (H * state.smoothedCovariance() * H.transpose() - V).inverse())
                  .eval();
          auto unbiasedParamsVec =
              state.smoothed() + K * (m - H * state.smoothed());
          auto unbiasedParamsCov =
              state.smoothedCovariance() - K * H * state.smoothedCovariance();
          return std::make_pair(unbiasedParamsVec, unbiasedParamsCov);
        }
        return std::nullopt;
      };


      // fill the fitted track parameters
      for (unsigned int ipar = 0; ipar < eSize; ++ipar) {
        // get the fitted track parameters
        auto trackParamsOpt = getTrackParams(ipar); 
        // fill the track parameters status
        m_hasParams[ipar].push_back(trackParamsOpt.has_value());

        if (!trackParamsOpt) {
          if (ipar == ePredicted) {
            // push default values if no track parameters
            m_res_x_hit.push_back(nan);
            m_res_y_hit.push_back(nan);
            m_err_x_hit.push_back(nan);
            m_err_y_hit.push_back(nan);
            m_pull_x_hit.push_back(nan);
            m_pull_y_hit.push_back(nan);
            m_dim_hit.push_back(std::numeric_limits<int>::quiet_NaN());
            m_index_hit.push_back(std::numeric_limits<int>::quiet_NaN());
          }
        // push default values if no track parameters
          m_eLOC0[ipar].push_back(nan);
          m_eLOC1[ipar].push_back(nan);
          m_ePHI[ipar].push_back(nan);
          m_eTHETA[ipar].push_back(nan);
          m_eQOP[ipar].push_back(nan);
          m_eT[ipar].push_back(nan);
          m_res_eLOC0[ipar].push_back(nan);
          m_res_eLOC1[ipar].push_back(nan);
          m_res_ePHI[ipar].push_back(nan);
          m_res_eTHETA[ipar].push_back(nan);
          m_res_eQOP[ipar].push_back(nan);
          m_res_eT[ipar].push_back(nan);
          m_err_eLOC0[ipar].push_back(nan);
          m_err_eLOC1[ipar].push_back(nan);
          m_err_ePHI[ipar].push_back(nan);
          m_err_eTHETA[ipar].push_back(nan);
          m_err_eQOP[ipar].push_back(nan);
          m_err_eT[ipar].push_back(nan);
          m_pull_eLOC0[ipar].push_back(nan);
          m_pull_eLOC1[ipar].push_back(nan);
          m_pull_ePHI[ipar].push_back(nan);
          m_pull_eTHETA[ipar].push_back(nan);
          m_pull_eQOP[ipar].push_back(nan);
          m_pull_eT[ipar].push_back(nan);
          m_x[ipar].push_back(nan);
          m_y[ipar].push_back(nan);
          m_z[ipar].push_back(nan);
          m_px[ipar].push_back(nan);
          m_py[ipar].push_back(nan);
          m_pz[ipar].push_back(nan);
          m_pT[ipar].push_back(nan);
          m_eta[ipar].push_back(nan);

          continue;
        }

        ++m_nParams[ipar];
        const auto& [parameters, covariance] = *trackParamsOpt;

        // track parameters
        m_eLOC0[ipar].push_back(parameters[Acts::eBoundLoc0]);
        m_eLOC1[ipar].push_back(parameters[Acts::eBoundLoc1]);
        m_ePHI[ipar].push_back(parameters[Acts::eBoundPhi]);
        m_eTHETA[ipar].push_back(parameters[Acts::eBoundTheta]);
        m_eQOP[ipar].push_back(parameters[Acts::eBoundQOverP]);
        m_eT[ipar].push_back(parameters[Acts::eBoundTime]);

        // track parameters residual
        float resPhi;
        if (isMC) {
          m_res_eLOC0[ipar].push_back(parameters[Acts::eBoundLoc0] - truthLOC0);
          m_res_eLOC1[ipar].push_back(parameters[Acts::eBoundLoc1] - truthLOC1);
          resPhi = Acts::detail::difference_periodic<float>( parameters[Acts::eBoundPhi], truthPHI,
                                                             static_cast<float>(2 * M_PI));
          m_res_ePHI[ipar].push_back(resPhi);
          m_res_eTHETA[ipar].push_back(parameters[Acts::eBoundTheta] - truthTHETA);
          m_res_eQOP[ipar].push_back(parameters[Acts::eBoundQOverP] - truthQOP);
          m_res_eT[ipar].push_back(parameters[Acts::eBoundTime] - truthTIME);

          // track parameters error
          m_err_eLOC0[ipar].push_back(
              sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
          m_err_eLOC1[ipar].push_back(
              sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1)));
          m_err_ePHI[ipar].push_back(
              sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi)));
          m_err_eTHETA[ipar].push_back(
              sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta)));
          m_err_eQOP[ipar].push_back(
              sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP)));
          m_err_eT[ipar].push_back(
              sqrt(covariance(Acts::eBoundTime, Acts::eBoundTime)));

          // track parameters pull
          m_pull_eLOC0[ipar].push_back(
              (parameters[Acts::eBoundLoc0] - truthLOC0) /
              sqrt(covariance(Acts::eBoundLoc0, Acts::eBoundLoc0)));
          m_pull_eLOC1[ipar].push_back(
              (parameters[Acts::eBoundLoc1] - truthLOC1) /
              sqrt(covariance(Acts::eBoundLoc1, Acts::eBoundLoc1)));
          m_pull_ePHI[ipar].push_back(
              resPhi / sqrt(covariance(Acts::eBoundPhi, Acts::eBoundPhi)));
          m_pull_eTHETA[ipar].push_back(
              (parameters[Acts::eBoundTheta] - truthTHETA) /
              sqrt(covariance(Acts::eBoundTheta, Acts::eBoundTheta)));
          m_pull_eQOP[ipar].push_back(
              (parameters[Acts::eBoundQOverP] - truthQOP) /
              sqrt(covariance(Acts::eBoundQOverP, Acts::eBoundQOverP)));
          m_pull_eT[ipar].push_back(
              (parameters[Acts::eBoundTime] - truthTIME) /
              sqrt(covariance(Acts::eBoundTime, Acts::eBoundTime)));
        } 

        // further track parameter info
        Acts::FreeVector freeParams =
            Acts::detail::transformBoundToFreeParameters(surface, gctx,
                                                         parameters);
        m_x[ipar].push_back(freeParams[Acts::eFreePos0]);
        m_y[ipar].push_back(freeParams[Acts::eFreePos1]);
        m_z[ipar].push_back(freeParams[Acts::eFreePos2]);
        auto r = std::hypot(freeParams[Acts::eFreePos0], freeParams[Acts::eFreePos1]);
        if(r>rMax_fit[ipar]){
          rMax_fit[ipar] = r;
        }
        auto p = std::abs(1 / freeParams[Acts::eFreeQOverP]);
        m_px[ipar].push_back(p * freeParams[Acts::eFreeDir0]);
        m_py[ipar].push_back(p * freeParams[Acts::eFreeDir1]);
        m_pz[ipar].push_back(p * freeParams[Acts::eFreeDir2]);
        m_pT[ipar].push_back(p * std::hypot(freeParams[Acts::eFreeDir0],
                                            freeParams[Acts::eFreeDir1]));
        m_eta[ipar].push_back(Acts::VectorHelpers::eta(
            freeParams.segment<3>(Acts::eFreeDir0)));
      

        if (ipar == ePredicted) {
          // local hit residual info
          auto H = state.effectiveProjector();
          auto V = state.effectiveCalibratedCovariance();
          auto resCov = V + H * covariance * H.transpose();
          Acts::ActsDynamicVector res(state.calibratedSize());
          res.setZero();

          res = state.effectiveCalibrated() - H * parameters;

          m_res_x_hit.push_back(res[Acts::eBoundLoc0]);
          m_err_x_hit.push_back(
              std::sqrt(V(Acts::eBoundLoc0, Acts::eBoundLoc0)));
          m_pull_x_hit.push_back(
              res[Acts::eBoundLoc0] /
              std::sqrt(resCov(Acts::eBoundLoc0, Acts::eBoundLoc0)));

          if (state.calibratedSize() >= 2) {
            m_res_y_hit.push_back(res[Acts::eBoundLoc1]);
            m_err_y_hit.push_back(
                std::sqrt(V(Acts::eBoundLoc1, Acts::eBoundLoc1)));
            m_pull_y_hit.push_back(
                res[Acts::eBoundLoc1] /
                std::sqrt(resCov(Acts::eBoundLoc1, Acts::eBoundLoc1)));
          } else {
            m_res_y_hit.push_back(nan);
            m_err_y_hit.push_back(nan);
            m_pull_y_hit.push_back(nan);
          }

          m_dim_hit.push_back(state.calibratedSize());
          IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
          m_index_hit.push_back(sl.index());
        }

      } // loop over parameters

    }// loop over states 
   
    m_rMax_hit = rMax_hit;
    m_t_rMax = t_rMax;
    for(unsigned int i=0; i< eSize; ++i){
      m_rMax[i] = rMax_fit[i];
    }
 
    // fill the variables for one track to tree
    m_outputTree->Fill();

    // now reset
    m_t_x.clear();
    m_t_y.clear();
    m_t_z.clear();
    m_t_dx.clear();
    m_t_dy.clear();
    m_t_dz.clear();
    m_t_eLOC0.clear();
    m_t_eLOC1.clear();
    m_t_ePHI.clear();
    m_t_eTHETA.clear();
    m_t_eQOP.clear();
    m_t_eT.clear();

    m_volumeID.clear();
    m_layerID.clear();
    m_moduleID.clear();
    m_station.clear();
    m_layer.clear();
    m_phi_module.clear();
    m_eta_module.clear();
    m_side.clear();
    m_pathLength.clear();
    m_lx_hit.clear();
    m_ly_hit.clear();
    m_x_hit.clear();
    m_y_hit.clear();
    m_z_hit.clear();
    m_res_x_hit.clear();
    m_res_y_hit.clear();
    m_err_x_hit.clear();
    m_err_y_hit.clear();
    m_pull_x_hit.clear();
    m_pull_y_hit.clear();
    m_dim_hit.clear();
    m_index_hit.clear();

    for (unsigned int ipar = 0; ipar < eSize; ++ipar) {
      m_hasParams[ipar].clear();
      m_eLOC0[ipar].clear();
      m_eLOC1[ipar].clear();
      m_ePHI[ipar].clear();
      m_eTHETA[ipar].clear();
      m_eQOP[ipar].clear();
      m_eT[ipar].clear();
      m_res_eLOC0[ipar].clear();
      m_res_eLOC1[ipar].clear();
      m_res_ePHI[ipar].clear();
      m_res_eTHETA[ipar].clear();
      m_res_eQOP[ipar].clear();
      m_res_eT[ipar].clear();
      m_err_eLOC0[ipar].clear();
      m_err_eLOC1[ipar].clear();
      m_err_ePHI[ipar].clear();
      m_err_eTHETA[ipar].clear();
      m_err_eQOP[ipar].clear();
      m_err_eT[ipar].clear();
      m_pull_eLOC0[ipar].clear();
      m_pull_eLOC1[ipar].clear();
      m_pull_ePHI[ipar].clear();
      m_pull_eTHETA[ipar].clear();
      m_pull_eQOP[ipar].clear();
      m_pull_eT[ipar].clear();
      m_x[ipar].clear();
      m_y[ipar].clear();
      m_z[ipar].clear();
      m_px[ipar].clear();
      m_py[ipar].clear();
      m_pz[ipar].clear();
      m_eta[ipar].clear();
      m_pT[ipar].clear();
    }

    m_chi2.clear();
  }  // all tracks 

  return StatusCode::SUCCESS;
}
