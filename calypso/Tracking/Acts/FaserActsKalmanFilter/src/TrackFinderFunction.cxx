#include "TrackFinderFunction.h"


struct TrackFinderFunctionImpl
    final : public TrackFinderFunction {
  Acts::GainMatrixUpdater kfUpdater;
  Acts::GainMatrixSmoother kfSmoother;
  
  CKF trackFinder;

  TrackFinderFunctionImpl(CKF &&f) : trackFinder(std::move(f)) {} 
  
  template <typename calibrator_t>
  auto makeCkfOptions(const GeneralFitterOptions& options,
                     const calibrator_t& calibrator, const IndexSourceLinkAccessor& slAccessor, const Acts::MeasurementSelector& measSel) const {
 
    Acts::CombinatorialKalmanFilterExtensions<Acts::VectorMultiTrajectory>
        extensions;
    extensions.calibrator.connect<&MeasurementCalibratorAdapter::calibrate>(
        &calibrator);
    extensions.updater.connect<
        &Acts::GainMatrixUpdater::operator()<Acts::VectorMultiTrajectory>>(
        &kfUpdater);
    extensions.smoother.connect<
        &Acts::GainMatrixSmoother::operator()<Acts::VectorMultiTrajectory>>(
        &kfSmoother);
    extensions.measurementSelector
        .connect<&Acts::MeasurementSelector::select<Acts::VectorMultiTrajectory>>(
            &measSel);

    Acts::SourceLinkAccessorDelegate<IndexSourceLinkAccessor::Iterator>
        slAccessorDelegate;
    slAccessorDelegate.connect<&IndexSourceLinkAccessor::range>(&slAccessor);

    TrackFinderOptions ckfOptions(options.geoContext, options.magFieldContext, options.calibrationContext, slAccessorDelegate,
        extensions, options.propOptions, &(*options.referenceSurface));

    //todo: make this configurable
    ckfOptions.smoothingTargetSurfaceStrategy =
        Acts::CombinatorialKalmanFilterTargetSurfaceStrategy::first;

    return ckfOptions; 
  }

  TrackFinderResult operator()(
      const Acts::BoundTrackParameters& initialParameters,
      const GeneralFitterOptions& options,
      const MeasurementCalibratorAdapter& calibrator,
      const IndexSourceLinkAccessor& slAccessor,
      const Acts::MeasurementSelector& measSel, 
      FaserActsTrackContainer& tracks) const override {
    auto  ckfOptions = makeCkfOptions(options, calibrator, slAccessor, measSel);
    return trackFinder.findTracks(initialParameters, ckfOptions, tracks);
  }
};



std::shared_ptr<TrackFinderFunction> makeTrackFinderFunction(
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
    std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
    bool resolvePassive, bool resolveMaterial, bool resolveSensitive, 
    const Acts::Logger& logger) {
  Stepper stepper(std::move(magneticField));
  Navigator::Config cfg{std::move(trackingGeometry)};
  cfg.resolvePassive = resolvePassive; 
  cfg.resolveMaterial = resolveMaterial;
  cfg.resolveSensitive = resolveSensitive;
  Navigator navigator(cfg, logger.cloneWithSuffix("Navigator"));
  Propagator propagator(std::move(stepper), std::move(navigator),
                        logger.cloneWithSuffix("Propagator"));
  CKF trackFinder(std::move(propagator), logger.cloneWithSuffix("Finder"));

  // build the track finder functions. owns the track finder object.
  return std::make_shared<TrackFinderFunctionImpl>(std::move(trackFinder));
}

