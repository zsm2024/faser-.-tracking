// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Utilities/CalibrationContext.hpp"
#include "Acts/EventData/MultiTrajectory.hpp"
#include "Acts/EventData/SourceLink.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"

#include "FaserActsKalmanFilter/IndexSourceLink.h"

#include <cassert>
#include <vector>
#include <variant>

/// Variable measurement type that can contain all possible combinations.
using Measurement = Acts::BoundVariantMeasurement;
/// Container of measurements.
///
/// In contrast to the source links, the measurements themself must not be
/// orderable. The source links stored in the measurements are treated
/// as opaque here and no ordering is enforced on the stored measurements.
using MeasurementContainer = std::vector<Measurement>;
//@todo: just for testing for now. Use SCT_clusters for it
using ClusterContainer = std::vector<Measurement>;

/// Calibrator to convert an index source link to a measurement.
class MeasurementCalibrator {
public:
    /// Construct an invalid calibrator. Required to allow copying.
    MeasurementCalibrator() = default;

    /// Find the measurement corresponding to the source link.
    ///
    /// @param measurements The measurements container 
    /// @param clusters The clusters container 
    /// @param gctx The geometry context
    /// @param cctx The calbiration context
    /// @param sourceLink Input source link
    /// @param trackState The track state to calibrate 
    void calibrate(
                   const MeasurementContainer& measurements,
                   const ClusterContainer* /*clusters*/,
                   const Acts::GeometryContext& /*gctx*/, 
                   const Acts::CalibrationContext& /*cctx*/, 
                   const Acts::SourceLink& sourceLink,
                   Acts::VectorMultiTrajectory::TrackStateProxy& trackState) const {
        const IndexSourceLink& idxSourceLink = sourceLink.get<IndexSourceLink>();
        assert((idxSourceLink.index() < measurements.size()) and
               "Source link index is outside the container bounds");
        
        trackState.setUncalibratedSourceLink(sourceLink);
       
        std::visit(
            [&trackState](const auto& meas) {
              trackState.allocateCalibrated(meas.size());
              trackState.setCalibrated(meas);
            },
            (measurements)[idxSourceLink.index()]);
    }

};

// Adapter class that wraps a MeasurementCalibrator to conform to the
// core ACTS calibration interface
class MeasurementCalibratorAdapter {
 public:
  MeasurementCalibratorAdapter(const MeasurementCalibrator& calibrator,
                               const MeasurementContainer& measurements,
                               const ClusterContainer* clusters = nullptr)
                               : m_calibrator{calibrator}, m_measurements{measurements}, m_clusters{clusters} {}

  MeasurementCalibratorAdapter() = delete;

  void calibrate(const Acts::GeometryContext& gctx,
                 const Acts::CalibrationContext& cctx,
                 const Acts::SourceLink& sourceLink,
                 Acts::VectorMultiTrajectory::TrackStateProxy trackState) const {
    return m_calibrator.calibrate(m_measurements, m_clusters, gctx, cctx,
                                sourceLink, trackState);
  }

 private:
  const MeasurementCalibrator& m_calibrator;
  const MeasurementContainer& m_measurements;
  const ClusterContainer* m_clusters;
};

