#ifndef FASERACTSKALMANFILTER_TRACKFINDERFUNCTION_H
#define FASERACTSKALMANFILTER_TRACKFINDERFUNCTION_H


#include "FaserActsTrack.h"
#include "TrackFitterFunction.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/Measurement.h"


#include "Acts/Definitions/Direction.hpp"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackStatePropMask.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/TrackFinding/CombinatorialKalmanFilter.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "Acts/TrackFinding/MeasurementSelector.hpp"
#include "Acts/Utilities/Logger.hpp"

//namespace {

using Updater = Acts::GainMatrixUpdater;
using Smoother = Acts::GainMatrixSmoother;

using Stepper = Acts::EigenStepper<>;
using Navigator = Acts::Navigator;
using Propagator = Acts::Propagator<Stepper, Navigator>;
using CKF =
    Acts::CombinatorialKalmanFilter<Propagator, Acts::VectorMultiTrajectory>;


using TrackFinderOptions =
    Acts::CombinatorialKalmanFilterOptions<IndexSourceLinkAccessor::Iterator,
                                           Acts::VectorMultiTrajectory>;
using TrackFinderResult =
  Acts::Result<std::vector<FaserActsTrackContainer::TrackProxy>>;


class TrackFinderFunction {
 public:

  virtual ~TrackFinderFunction() = default;
  virtual TrackFinderResult operator()(
      const Acts::BoundTrackParameters& initialParameters,
      const GeneralFitterOptions& options,
      const MeasurementCalibratorAdapter& calibrator,
      const IndexSourceLinkAccessor& slAccessor,
      const Acts::MeasurementSelector& measSel,
      FaserActsTrackContainer& tracks) const = 0;
};


 std::shared_ptr<TrackFinderFunction> makeTrackFinderFunction(
  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
  std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
  bool resolvePassive, bool resolveMaterial, bool resolveSensitive,
  const Acts::Logger& logger);


//}  // namespace

#endif  // FASERACTSKALMANFILTER_TRACKFINDERFUNCTION_H

