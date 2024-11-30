// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "FaserActsGeometry/CuboidVolumeBuilder.h"

#include "Acts/Geometry/CuboidVolumeBounds.hpp"
#include "Acts/Geometry/Layer.hpp"
#include "Acts/Geometry/LayerArrayCreator.hpp"
#include "Acts/Geometry/LayerCreator.hpp"
#include "Acts/Geometry/PlaneLayer.hpp"
#include "Acts/Geometry/SurfaceArrayCreator.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Geometry/TrackingVolume.hpp"
//#include "Acts/Geometry/detail/DefaultDetectorElementBase.hpp"
#include "Acts/Geometry/ApproachDescriptor.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Material/Material.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/SurfaceArray.hpp"
#include "Acts/Utilities/BinnedArray.hpp"
#include "Acts/Utilities/BinnedArrayXD.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Definitions/Algebra.hpp"


namespace FaserActs{

std::shared_ptr<const Acts::PlaneSurface>
CuboidVolumeBuilder::buildSurface(
    const Acts::GeometryContext& /*gctx*/,
    const CuboidVolumeBuilder::SurfaceConfig& cfg) const {
  std::shared_ptr<Acts::PlaneSurface> surface;

  // Build transformation
  Acts::Transform3 trafo(Acts::Transform3::Identity() * cfg.rotation);
  trafo.translation() = cfg.position;

  // Create and store surface
  if (cfg.detElementConstructor) {
    surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
        cfg.rBounds,
        *(cfg.detElementConstructor(std::make_shared<const Acts::Transform3>(trafo),
                                    cfg.rBounds, cfg.thickness)));
  } else {
    surface = Acts::Surface::makeShared<Acts::PlaneSurface>(
        trafo, cfg.rBounds);
  }
  surface->assignSurfaceMaterial(cfg.surMat);
  return surface;
}

std::shared_ptr<const Acts::Layer> CuboidVolumeBuilder::buildLayer(
    const Acts::GeometryContext& gctx,
    CuboidVolumeBuilder::LayerConfig& cfg) const {
  // Build the surface
  if ( cfg.surface==nullptr && cfg.surfaces.empty() ) {
    cfg.surface = buildSurface(gctx, cfg.surfaceCfg);
  }
  // Build transformation centered at the surface position
  Acts::Transform3 trafo(Acts::Transform3::Identity() * cfg.surfaceCfg.rotation);
  trafo.translation() = cfg.surfaceCfg.position;

  Acts::LayerCreator::Config lCfg;
  lCfg.surfaceArrayCreator = std::make_shared<const Acts::SurfaceArrayCreator>();
  Acts::LayerCreator layerCreator(lCfg);

  std::unique_ptr<Acts::ApproachDescriptor> ap(cfg.approachDescriptor);

  if ( !cfg.surfaces.empty() ) {
    return layerCreator.planeLayer(gctx, cfg.surfaces, cfg.binsX, cfg.binsY,
                                 Acts::BinningValue::binZ, std::nullopt, trafo,
				 std::move(ap));
  } else {
    return layerCreator.planeLayer(gctx, {cfg.surface}, cfg.binsX, cfg.binsY,
                                 Acts::BinningValue::binZ, std::nullopt, trafo, std::move(ap));
  }
}

std::pair<double, double> CuboidVolumeBuilder::binningRange(
    const Acts::GeometryContext& /*gctx*/,
    const CuboidVolumeBuilder::VolumeConfig& cfg) const {
  using namespace Acts::UnitLiterals;
  // Construct return value
  std::pair<double, double> minMax = std::make_pair(
      std::numeric_limits<double>::max(), -std::numeric_limits<double>::max());
  for (const auto& layercfg : cfg.layerCfg) {
    // Test if new extreme is found and set it
    
    // For Faser, the thickness of each layer is about 5 mm due to staggering structure of SCTs
    //if (layercfg.surfaceCfg.position.z() - 1_um < minMax.first) {
      //minMax.first = layercfg.surfaceCfg.position.z() - 1_um;
    if (layercfg.surfaceCfg.position.z() - 8_mm < minMax.first) {
      minMax.first = layercfg.surfaceCfg.position.z() - 8_mm;
    }
    //if (layercfg.surfaceCfg.position.z() + 1_um > minMax.second) {
      //minMax.second = layercfg.surfaceCfg.position.z() + 1_um;
    if (layercfg.surfaceCfg.position.z() + 8_mm > minMax.second) {
      minMax.second = layercfg.surfaceCfg.position.z() + 8_mm;
    }
  }
  return minMax;
}

std::shared_ptr<Acts::TrackingVolume> CuboidVolumeBuilder::buildVolume(
    const Acts::GeometryContext& gctx,
    CuboidVolumeBuilder::VolumeConfig& cfg) const {
  // Build transformation
  Acts::Transform3 trafo(Acts::Transform3::Identity());
  trafo.translation() = cfg.position;
  // Set bounds
  auto bounds = std::make_shared<const Acts::CuboidVolumeBounds>(
      cfg.length.x() * 0.5, cfg.length.y() * 0.5, cfg.length.z() * 0.5);

  if (cfg.layerCfg.empty()) {
    // Build dummy layer if no layer is given (tmp solution)
    SurfaceConfig sCfg;
    sCfg.position = cfg.position;
    // Rotation of the surfaces: +pi/2 around axis y
    //Acts::Vector3 xPos(0., 0., 1.);
    //Acts::Vector3 yPos(0., 1., 0.);
    //Acts::Vector3 zPos(-1., 0., 0.);
    //sCfg.rotation.col(0) = xPos;
    //sCfg.rotation.col(1) = yPos;
    //sCfg.rotation.col(2) = zPos;
    // Bounds
    sCfg.rBounds = std::make_shared<const Acts::RectangleBounds>(
        Acts::RectangleBounds(cfg.length.x() * 0.5, cfg.length.y() * 0.5));

    LayerConfig lCfg;
    lCfg.surfaceCfg = sCfg;

    cfg.layerCfg.push_back(lCfg);
  }

  // Gather the layers
  Acts::LayerVector layVec;
  if (cfg.layers.empty()) {
    cfg.layers.reserve(cfg.layerCfg.size());

    for (auto& layerCfg : cfg.layerCfg) {
      cfg.layers.push_back(buildLayer(gctx, layerCfg));
      layVec.push_back(cfg.layers.back());
    }
  } else {
    for (auto& lay : cfg.layers) {
      layVec.push_back(lay);
    }
  }

  // Build layer array
  std::pair<double, double> minMax = binningRange(gctx, cfg);
  Acts::LayerArrayCreator::Config lacCnf;
  Acts::LayerArrayCreator layArrCreator(
      lacCnf, Acts::getDefaultLogger("Acts::LayerArrayCreator", Acts::Logging::INFO));
  std::unique_ptr<const Acts::LayerArray> layArr(
      layArrCreator.layerArray(gctx, layVec, minMax.first, minMax.second,
                               Acts::BinningType::arbitrary, Acts::BinningValue::binZ));

  // Build confined volumes
     if (cfg.trackingVolumes.empty())
       for (VolumeConfig vc : cfg.volumeCfg)            
	  cfg.trackingVolumes.push_back(buildVolume(gctx, vc));
               
  // Build TrackingVolume
  auto trackVolume = Acts::TrackingVolume::create(
      trafo, bounds, cfg.volumeMaterial, std::move(layArr), nullptr, cfg.trackingVolumes, cfg.name);
  
  return trackVolume;
}

Acts::MutableTrackingVolumePtr CuboidVolumeBuilder::trackingVolume(
    const Acts::GeometryContext& gctx, Acts::TrackingVolumePtr /*unused*/,
    Acts::VolumeBoundsPtr /*unused*/) const {
  // Build volumes
  std::vector<std::shared_ptr<Acts::TrackingVolume>> volumes;
  volumes.reserve(m_cfg.volumeCfg.size());
  for (VolumeConfig volCfg : m_cfg.volumeCfg) {
    volumes.push_back(buildVolume(gctx, volCfg));
  }

  // Glue volumes
  for (unsigned int i = 0; i < volumes.size() - 1; i++) {
    volumes[i + 1]->glueTrackingVolume(
        gctx, Acts::BoundarySurfaceFace::negativeFaceXY, volumes[i].get(),
        Acts::BoundarySurfaceFace::positiveFaceXY);
    volumes[i]->glueTrackingVolume(gctx, Acts::BoundarySurfaceFace::positiveFaceXY,
                                   volumes[i + 1].get(),
                                   Acts::BoundarySurfaceFace::negativeFaceXY);
  }

  // Translation
  Acts::Transform3 trafo(Acts::Transform3::Identity());
  trafo.translation() = m_cfg.position;

  // Size of the volume
  auto volume = std::make_shared<const Acts::CuboidVolumeBounds>(
      m_cfg.length.x() * 0.5, m_cfg.length.y() * 0.5, m_cfg.length.z() * 0.5);

  // Build vector of confined volumes
  std::vector<std::pair<Acts::TrackingVolumePtr, Acts::Vector3>> tapVec;
  tapVec.reserve(m_cfg.volumeCfg.size());
  for (auto& tVol : volumes) {
    tapVec.push_back(std::make_pair(tVol, tVol->center()));
  }

  // Set bin boundaries along binning
  std::vector<float> binBoundaries;
  binBoundaries.push_back(volumes[0]->center().z() -
                          m_cfg.volumeCfg[0].length.z() * 0.5);
  for (size_t i = 0; i < volumes.size(); i++) {
    binBoundaries.push_back(volumes[i]->center().z() +
                            m_cfg.volumeCfg[i].length.z() * 0.5);
  }

  // Build binning
  Acts::BinningData binData(Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
  auto bu = std::make_unique<const Acts::BinUtility>(binData);

  // Build TrackingVolume array
  std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(
      new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

  // Create world volume
  Acts::MutableTrackingVolumePtr mtvp(Acts::TrackingVolume::create(
      trafo, volume, trVolArr, "World"));

  return mtvp;
}

}
