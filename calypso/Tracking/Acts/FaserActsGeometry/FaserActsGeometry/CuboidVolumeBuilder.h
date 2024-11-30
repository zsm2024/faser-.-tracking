// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/ITrackingVolumeBuilder.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Geometry/GenericApproachDescriptor.hpp"

namespace Acts{

class TrackingVolume;
class VolumeBounds;
class RectangleBounds;
class ISurfaceMaterial;
class IVolumeMaterial;
class DetectorElementBase;
class PlaneSurface;
class Surface;

}

namespace FaserActs{

/// @brief This class builds a box detector with a configurable amount of
/// surfaces in it. The idea is to allow a quick configuration of a detector for
/// mostly unit test applications. Therefore this class does not demand to be a
/// universal construction factory but a raw first draft of the idea of factory
/// that may be extended in the future.
class CuboidVolumeBuilder : public Acts::ITrackingVolumeBuilder {
 public:
  /// @brief This struct stores the data for the construction of a single
  /// PlaneSurface
  struct SurfaceConfig {
    // Center position
    Acts::Vector3 position;
    // Rotation
    Acts::RotationMatrix3 rotation = Acts::RotationMatrix3::Identity();
    // Bounds
    std::shared_ptr<const Acts::RectangleBounds> rBounds = nullptr;
    // Attached material
    std::shared_ptr<const Acts::ISurfaceMaterial> surMat = nullptr;
    // Thickness
    double thickness = 0.;
    // Constructor function for optional detector elements
    // Arguments are transform, rectangle bounds and thickness.
    std::function<Acts::DetectorElementBase*(std::shared_ptr<const Acts::Transform3>,
                                       std::shared_ptr<const Acts::RectangleBounds>,
                                       double)>
        detElementConstructor;
  };

  /// @brief This struct stores the data for the construction of a PlaneLayer
  /// that has a single PlaneSurface encapsulated or array of surfaces
  struct LayerConfig {
    // Configuration of the surface
    SurfaceConfig surfaceCfg;
    // Encapsulated surface
    std::shared_ptr<const Acts::PlaneSurface> surface = nullptr;
    std::vector<std::shared_ptr<const Acts::Surface>> surfaces;
    // Boolean flag if layer is active
    bool active = false;
    // Bins in X direction
    size_t binsX = 1;
    // Bins in Y direction
    size_t binsY = 1;
    // The approach descriptor
    Acts::GenericApproachDescriptor *approachDescriptor = nullptr;
  };

  /// @brief This struct stores the data for the construction of a cuboid
  /// TrackingVolume with a given number of PlaneLayers
  struct VolumeConfig {
    // Center position
    Acts::Vector3 position;
    // Lengths in x,y,z
    Acts::Vector3 length;
    // Configurations of its layers
    std::vector<LayerConfig> layerCfg;
    // Stored layers
    std::vector<std::shared_ptr<const Acts::Layer>> layers;
    // Configurations of confined volumes
    std::vector<VolumeConfig> volumeCfg;
    // Stored confined volumes
    std::vector<std::shared_ptr<Acts::TrackingVolume>> trackingVolumes;
    // Name of the volume
    std::string name = "Volume";
    // Material
    std::shared_ptr<const Acts::IVolumeMaterial> volumeMaterial = nullptr;
  };

  /// @brief This struct stores the configuration of the tracking geometry
  struct Config {
    // Center position
    Acts::Vector3 position = Acts::Vector3(0., 0., 0.);
    // Length in x,y,z
    Acts::Vector3 length = Acts::Vector3(0., 0., 0.);
    // Configuration of its volumes
    std::vector<VolumeConfig> volumeCfg = {};
  };

  /// @brief Default constructor without a configuration
  CuboidVolumeBuilder() = default;

  /// @brief Constructor that sets the config
  ///
  /// @param [in] cfg Configuration of the detector
  CuboidVolumeBuilder(Config& cfg) : m_cfg(cfg) {}

  /// @brief Setter of the config
  ///
  /// @param [in] cfg Configuration that is set
  void setConfig(Config& cfg) { m_cfg = cfg; }

  /// @brief This function creates a surface with a given configuration. A
  /// detector element is attached if the template parameter is non-void.
  ///
  /// @param [in] gctx the geometry context for this building
  /// @param [in] cfg Configuration of the surface
  ///
  /// @return Pointer to the created surface
  std::shared_ptr<const Acts::PlaneSurface> buildSurface(
      const Acts::GeometryContext& gctx, const SurfaceConfig& cfg) const;

  /// @brief This function creates a layer with a surface encaspulated with a
  /// given configuration. The surface gets a detector element attached if the
  /// template parameter is non-void.
  ///
  /// @param [in] gctx the geometry context for this building
  /// @param [in, out] cfg Configuration of the layer and the surface
  ///
  /// @return Pointer to the created layer
  std::shared_ptr<const Acts::Layer> buildLayer(const Acts::GeometryContext& gctx,
                                          LayerConfig& cfg) const;

  /// @brief This function creates a TrackingVolume with a configurable number
  /// of layers and surfaces. Each surface gets a detector element attached if
  /// the template parameter is non-void.
  ///
  /// @param [in] gctx the geometry context for this building
  /// @param [in, out] cfg Configuration of the TrackingVolume
  ///
  /// @return Pointer to the created TrackingVolume
  std::shared_ptr<Acts::TrackingVolume> buildVolume(const Acts::GeometryContext& gctx,
                                              VolumeConfig& cfg) const;

  /// @brief This function evaluates the minimum and maximum of the binning as
  /// given by the configurations of the surfaces and layers. The ordering
  /// depends on the binning value specified in the configuration of the volume.
  ///
  /// @param [in] gctx the geometry context for this building
  /// @param [in] cfg Container with the given surfaces and layers
  ///
  /// @return Pair containing the minimum and maximum along the binning
  /// direction
  std::pair<double, double> binningRange(const Acts::GeometryContext& gctx,
                                         const VolumeConfig& cfg) const;

  /// @brief This function builds a world TrackingVolume based on a given
  /// configuration
  ///
  /// @param [in] gctx the geometry context for this building
  ///
  /// @return Pointer to the created TrackingGeometry
  std::shared_ptr<Acts::TrackingVolume> trackingVolume(
      const Acts::GeometryContext& gctx,
      std::shared_ptr<const Acts::TrackingVolume> /*unused*/,
      std::shared_ptr<const Acts::VolumeBounds> /*unused*/) const override;

 private:
  /// Configuration of the world volume
  Config m_cfg;
};

}  // namespace Acts
