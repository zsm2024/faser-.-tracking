#ifndef FASERACTSKALMANFILTER_TRACKFITTERFUNCTION_H
#define FASERACTSKALMANFILTER_TRACKFITTERFUNCTION_H

// Amg

// Ensure Eigen plugin comes first
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

#include "FaserActsTrack.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/Measurement.h"

#include "Acts/Definitions/Direction.hpp"
#include "Acts/Definitions/TrackParametrization.hpp"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackStatePropMask.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/EventData/detail/CorrectedTransformationFreeToBound.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Propagator/DirectNavigator.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/TrackFitting/GainMatrixSmoother.hpp"
#include "Acts/TrackFitting/GainMatrixUpdater.hpp"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/Utilities/Delegate.hpp"
#include "Acts/Utilities/Logger.hpp"


//namespace  {

using Stepper = Acts::EigenStepper<>;
using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;
using Fitter = Acts::KalmanFitter<Propagator, Acts::VectorMultiTrajectory>;


struct GeneralFitterOptions {
  std::reference_wrapper<const Acts::GeometryContext> geoContext;
  std::reference_wrapper<const Acts::MagneticFieldContext> magFieldContext;
  std::reference_wrapper<const Acts::CalibrationContext> calibrationContext;
  const Acts::Surface* referenceSurface = nullptr;
  Acts::PropagatorPlainOptions propOptions;
};


struct SimpleReverseFilteringLogic {
  double momentumThreshold = 0;

  bool doBackwardFiltering(
      Acts::VectorMultiTrajectory::ConstTrackStateProxy trackState) const {
    auto momentum = fabs(1 / trackState.filtered()[Acts::eBoundQOverP]);
    return (momentum <= momentumThreshold);
  }
};


//set the cluster to be removed as outlier in order to get the unbiased residual
/// Outlier finder using a Chi2 cut.
struct FaserActsOutlierFinder {
  double StateChiSquaredPerNumberDoFCut = 10000.;
  double cluster_z = -10000.;

  template <typename traj_t>
  bool operator()(typename traj_t::ConstTrackStateProxy state) const;
};


/// Fit function that takes the above parameters and runs a fit
/// @note This is separated into a virtual interface to keep compilation units
/// small.
class TrackFitterFunction {
 public:
  using TrackFitterResult = Acts::Result<FaserActsTrackContainer::TrackProxy>;

  virtual ~TrackFitterFunction() = default;

  virtual TrackFitterResult operator()(const std::vector<Acts::SourceLink>&,
                                       const Acts::BoundTrackParameters&,
                                       const GeneralFitterOptions&,
                                       const MeasurementCalibratorAdapter&,
                                       FaserActsTrackContainer&) const = 0;

  virtual TrackFitterResult operator()(const std::vector<Acts::SourceLink>&,
                                       const Acts::BoundTrackParameters&,
                                       const GeneralFitterOptions&,
                                       const MeasurementCalibratorAdapter&,
                                       const FaserActsOutlierFinder& outlierFinder, 
                                       FaserActsTrackContainer&) const = 0;

};
  

std::shared_ptr<TrackFitterFunction> makeTrackFitterFunction(
std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry,
std::shared_ptr<const Acts::MagneticFieldProvider> magneticField,
bool multipleScattering, bool energyLoss,
double reverseFilteringMomThreshold,
Acts::FreeToBoundCorrection freeToBoundCorrection,
const Acts::Logger& logger);


//}  // namespace

#endif  // FASERACTSKALMANFILTER_TRACKFITTERFUNCTION_H
