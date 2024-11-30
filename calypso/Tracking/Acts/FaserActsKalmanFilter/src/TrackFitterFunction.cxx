#include "TrackFitterFunction.h"
#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"

template <typename traj_t>
bool FaserActsOutlierFinder::operator()(typename traj_t::ConstTrackStateProxy state) const {
    //remove the whole IFT
    if(cluster_z<-10000){
      IndexSourceLink sl = state.getUncalibratedSourceLink().template get<IndexSourceLink>();
      const Tracker::FaserSCT_Cluster* cluster = sl.hit(); 
      if(cluster->globalPosition().z()<-100)return true;
      if(abs(sl.hit()->globalPosition().z()-cluster_z)<3)return true;
    }

    if (not state.hasCalibrated() or not state.hasPredicted()) {
      return false;
    }

    return bool(state.chi2() > StateChiSquaredPerNumberDoFCut * state.calibratedSize());
  }


struct TrackFitterFunctionImpl final : public TrackFitterFunction {
  Fitter trackFitter;

  Acts::GainMatrixUpdater kfUpdater;
  Acts::GainMatrixSmoother kfSmoother;
  SimpleReverseFilteringLogic reverseFilteringLogic;

  bool multipleScattering = false;
  bool energyLoss = false;
  Acts::FreeToBoundCorrection freeToBoundCorrection;

  IndexSourceLink::SurfaceAccessor slSurfaceAccessor;

  TrackFitterFunctionImpl(Fitter &&f, const Acts::TrackingGeometry& trkGeo) : trackFitter(std::move(f)),slSurfaceAccessor{trkGeo} {}

  template <typename calibrator_t>
  auto makeKfOptions(const GeneralFitterOptions& options,
                     const calibrator_t& calibrator) const {
    Acts::KalmanFitterExtensions<Acts::VectorMultiTrajectory> extensions;
    extensions.updater.connect<
        &Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(
        &kfUpdater);
    extensions.smoother.connect<
        &Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(
        &kfSmoother);
    extensions.reverseFilteringLogic
        .connect<&SimpleReverseFilteringLogic::doBackwardFiltering>(
            &reverseFilteringLogic);

    Acts::KalmanFitterOptions<Acts::VectorMultiTrajectory> kfOptions(
        options.geoContext, options.magFieldContext, options.calibrationContext,
        extensions, options.propOptions, &(*options.referenceSurface));

    kfOptions.referenceSurfaceStrategy =
        Acts::KalmanFitterTargetSurfaceStrategy::first;
    kfOptions.multipleScattering = multipleScattering;
    kfOptions.energyLoss = energyLoss;
    kfOptions.freeToBoundCorrection = freeToBoundCorrection;
    kfOptions.extensions.calibrator.connect<&calibrator_t::calibrate>(
        &calibrator);
    kfOptions.extensions.surfaceAccessor
        .connect<&IndexSourceLink::SurfaceAccessor::operator()>(
            &slSurfaceAccessor);

    return kfOptions;
  }


  template <typename calibrator_t>
  auto makeKfOptions(const GeneralFitterOptions& options,
                     const calibrator_t& calibrator, const FaserActsOutlierFinder& outlierFinder) const {
    Acts::KalmanFitterExtensions<Acts::VectorMultiTrajectory> extensions;
    extensions.updater.connect<
        &Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(
        &kfUpdater);
    extensions.smoother.connect<
        &Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(
        &kfSmoother);
    extensions.reverseFilteringLogic
        .connect<&SimpleReverseFilteringLogic::doBackwardFiltering>(
            &reverseFilteringLogic);
    extensions.outlierFinder.connect<&FaserActsOutlierFinder::operator()<Acts::VectorMultiTrajectory>>(
            &outlierFinder);

    Acts::KalmanFitterOptions<Acts::VectorMultiTrajectory> kfOptions(
        options.geoContext, options.magFieldContext, options.calibrationContext,
        extensions, options.propOptions, &(*options.referenceSurface));

    kfOptions.referenceSurfaceStrategy =
        Acts::KalmanFitterTargetSurfaceStrategy::first;
    kfOptions.multipleScattering = multipleScattering;
    kfOptions.energyLoss = energyLoss;
    kfOptions.freeToBoundCorrection = freeToBoundCorrection;
    kfOptions.extensions.calibrator.connect<&calibrator_t::calibrate>(
        &calibrator);
    kfOptions.extensions.surfaceAccessor
        .connect<&IndexSourceLink::SurfaceAccessor::operator()>(
            &slSurfaceAccessor);

    return kfOptions;
  }


  //fit with default outlier finder, i.e. not finding outlier
  TrackFitterResult operator()(
      const std::vector<Acts::SourceLink> &sourceLinks,
      const Acts::BoundTrackParameters &initialParameters,
      const GeneralFitterOptions& options,
      const MeasurementCalibratorAdapter& calibrator,
      FaserActsTrackContainer& tracks) const override {
    const auto kfOptions = makeKfOptions(options, calibrator);
    return trackFitter.fit(sourceLinks.begin(), sourceLinks.end(), initialParameters,
                      kfOptions, tracks);
  }

  
  //fit with faser outlier finder
  TrackFitterResult operator()(
      const std::vector<Acts::SourceLink> &sourceLinks,
      const Acts::BoundTrackParameters &initialParameters,
      const GeneralFitterOptions& options,
      const MeasurementCalibratorAdapter& calibrator,
      const FaserActsOutlierFinder& outlierFinder, 
      FaserActsTrackContainer& tracks) const override {
    const auto kfOptions = makeKfOptions(options, calibrator, outlierFinder);
    return trackFitter.fit(sourceLinks.begin(), sourceLinks.end(), initialParameters,
                      kfOptions, tracks);
  }

};


std::shared_ptr<TrackFitterFunction> makeTrackFitterFunction(
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
    bool multipleScattering, bool energyLoss,
    double reverseFilteringMomThreshold,
    Acts::FreeToBoundCorrection freeToBoundCorrection,
    const Acts::Logger& logger) {
  // Stepper should be copied into the fitters
  const Stepper stepper(std::move(magneticField));

  // Standard fitter
  const auto& geo = *trackingGeometry;
  Acts::Navigator::Config cfg{std::move(trackingGeometry)};
  cfg.resolvePassive = false;
  cfg.resolveMaterial = true;
  cfg.resolveSensitive = true;
  Acts::Navigator navigator(cfg, logger.cloneWithSuffix("Navigator"));
  Propagator propagator(stepper, std::move(navigator),
                        logger.cloneWithSuffix("Propagator"));
  Fitter trackFitter(std::move(propagator), logger.cloneWithSuffix("Fitter"));

  // build the fitter function. owns the fitter object.
  auto fitterFunction = std::make_shared<TrackFitterFunctionImpl>(
      std::move(trackFitter), geo);
  fitterFunction->multipleScattering = multipleScattering;
  fitterFunction->energyLoss = energyLoss;
  fitterFunction->reverseFilteringLogic.momentumThreshold =
      reverseFilteringMomThreshold;
  fitterFunction->freeToBoundCorrection = freeToBoundCorrection;

  return fitterFunction;
}

