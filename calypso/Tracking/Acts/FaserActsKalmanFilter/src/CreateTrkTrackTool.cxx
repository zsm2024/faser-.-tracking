#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "CreateTrkTrackTool.h"

#include <Acts/EventData/MultiTrajectoryHelpers.hpp>
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"

CreateTrkTrackTool::CreateTrkTrackTool(const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool( type, name, parent ) {}

StatusCode CreateTrkTrackTool::initialize() {
  ATH_CHECK(detStore()->retrieve(m_idHelper,"FaserSCT_ID"));
  return StatusCode::SUCCESS;
}

StatusCode CreateTrkTrackTool::finalize() {
  return StatusCode::SUCCESS;
}

std::unique_ptr<Trk::Track>
CreateTrkTrackTool::createTrack(const Acts::GeometryContext &gctx, const CreateTrkTrackTool::TrackContainer::TrackProxy &track, const std::optional<Acts::BoundTrackParameters>& fittedParams, bool backwardPropagation) const {
  std::unique_ptr<Trk::Track> newtrack = nullptr;
  DataVector<const Trk::TrackStateOnSurface>* finalTrajectory = new DataVector<const Trk::TrackStateOnSurface>{};
  // Loop over all the output state to create track state
  for (const auto& state : track.trackStatesReversed()) {
    auto flag = state.typeFlags();
    if (state.referenceSurface().associatedDetectorElement() != nullptr) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
      std::unique_ptr<Trk::TrackParameters> parm;

      if (flag.test(Acts::TrackStateFlag::HoleFlag)) {
        //todo: make the particle hypothesis configurable
        const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                   state.predicted(), state.predictedCovariance(), Acts::ParticleHypothesis::muon());
        parm = std::move(ConvertActsTrackParameterToATLAS(actsParam, gctx));
        typePattern.set(Trk::TrackStateOnSurface::Hole);
      }
      else if (flag.test(Acts::TrackStateFlag::OutlierFlag)) {
        const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                   state.filtered(), state.filteredCovariance(), Acts::ParticleHypothesis::muon());
        parm = std::move(ConvertActsTrackParameterToATLAS(actsParam, gctx));
        typePattern.set(Trk::TrackStateOnSurface::Outlier);
      }
      else {
        const Acts::BoundTrackParameters actsParam(state.referenceSurface().getSharedPtr(),
                                                   state.smoothed(), state.smoothedCovariance(), Acts::ParticleHypothesis::muon());
        parm = std::move(ConvertActsTrackParameterToATLAS(actsParam, gctx));
        typePattern.set(Trk::TrackStateOnSurface::Measurement);
      }
      std::unique_ptr<Tracker::FaserSCT_ClusterOnTrack> clusterOnTrack = nullptr;
      if (state.hasUncalibratedSourceLink()) {
        const Tracker::FaserSCT_Cluster* cluster = state.getUncalibratedSourceLink().template get<IndexSourceLink>().hit();
        if (cluster->detectorElement() != nullptr) {
          clusterOnTrack = std::make_unique<Tracker::FaserSCT_ClusterOnTrack>(
              cluster,
              Trk::LocalParameters{
                  Trk::DefinedParameter{cluster->localPosition()[0], Trk::loc1},
                  Trk::DefinedParameter{cluster->localPosition()[1], Trk::loc2}
              },
              Amg::MatrixX(cluster->localCovariance()),
              m_idHelper->wafer_hash(cluster->detectorElement()->identify())
          );
        }
      }
      double nDoF = state.calibratedSize();
      const Trk::FitQualityOnSurface* quality = new Trk::FitQualityOnSurface(state.chi2(), nDoF);
      const Trk::TrackStateOnSurface* perState = new Trk::TrackStateOnSurface(
									      *quality,
									      std::move(clusterOnTrack), 
									      std::move(parm),
									      nullptr,
									      typePattern);
      if ((perState) && (!backwardPropagation)) {
	        finalTrajectory->insert(finalTrajectory->begin(), perState);
      } else if ((perState) && (backwardPropagation)) {
	        finalTrajectory->push_back(perState);
      }
    }
  }

  // create track state for the fitted track parameters at target surface and type it as hole
  if(fittedParams.has_value()){ 
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type;
    type.set(Trk::TrackStateOnSurface::Hole);
    std::unique_ptr<Trk::TrackParameters> param = std::move(ConvertActsTrackParameterToATLAS(fittedParams.value(), gctx));
    Trk::FitQualityOnSurface* quality = new Trk::FitQualityOnSurface(-99, 0);  
    const Trk::TrackStateOnSurface* fittedParamsState = new Trk::TrackStateOnSurface(*quality, nullptr, std::move(param), nullptr, type); 
    if (!backwardPropagation) {
      finalTrajectory->insert(finalTrajectory->begin(), fittedParamsState);
    } else if (backwardPropagation) {
      finalTrajectory->push_back(fittedParamsState);
    }
  }

  std::unique_ptr<Trk::FitQuality> q = std::make_unique<Trk::FitQuality>(track.chi2(), static_cast<double>(track.nMeasurements() - 5));
  Trk::TrackInfo newInfo(Trk::TrackInfo::TrackFitter::KalmanFitter, Trk::ParticleHypothesis::muon);
  std::unique_ptr<DataVector<const Trk::TrackStateOnSurface>> sink(finalTrajectory);
  Trk::Track* tmpTrack = new Trk::Track(newInfo, std::move(sink), std::move(q));
  newtrack = std::unique_ptr<Trk::Track>(tmpTrack);
  return newtrack;
}


std::unique_ptr<Trk::TrackParameters>
CreateTrkTrackTool::ConvertActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const {
  using namespace Acts::UnitLiterals;
  std::optional<AmgSymMatrix(5)> cov = std::nullopt;
  if (actsParameter.covariance()){
    AmgSymMatrix(5) newcov(actsParameter.covariance()->topLeftCorner(5, 5));
    // Convert the covariance matrix to GeV
    for(int i=0; i < newcov.rows(); i++){
      newcov(i, 4) = newcov(i, 4)*1_MeV;
    }
    for(int i=0; i < newcov.cols(); i++){
      newcov(4, i) = newcov(4, i)*1_MeV;
    }
    cov =  std::optional<AmgSymMatrix(5)>(newcov);
  }
  const Amg::Vector3D& pos=actsParameter.position(gctx);
  double tphi=actsParameter.get<Acts::eBoundPhi>();
  double ttheta=actsParameter.get<Acts::eBoundTheta>();
  double tqOverP=actsParameter.get<Acts::eBoundQOverP>()*1_MeV;
  double p = std::abs(1. / tqOverP);
  Amg::Vector3D tmom(p * std::cos(tphi) * std::sin(ttheta), p * std::sin(tphi) * std::sin(ttheta), p * std::cos(ttheta));
  double charge = tqOverP > 0. ? 1. : -1.;
  return std::make_unique<Trk::CurvilinearParameters>(pos,tmom,charge, cov);
}
