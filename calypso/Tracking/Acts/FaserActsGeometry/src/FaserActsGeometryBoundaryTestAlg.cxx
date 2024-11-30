/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS and FASER collaborations
*/


#include "FaserActsGeometryBoundaryTestAlg.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/TrackingVolume.hpp"


FaserActsGeometryBoundaryTestAlg::FaserActsGeometryBoundaryTestAlg(const std::string &name, ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator) {}


StatusCode FaserActsGeometryBoundaryTestAlg::initialize() {
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  return StatusCode::SUCCESS;
}


StatusCode FaserActsGeometryBoundaryTestAlg::execute(const EventContext &ctx) const {

  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
      = m_trackingGeometryTool->trackingGeometry();

  const Acts::GeometryContext gctx =
      m_trackingGeometryTool->getGeometryContext(ctx).context();

  // loop over all tracking layer and check if contained detector elements are insie boundaries
  // return StatusCode::Failure so that test fails, if the edge of any detector element is outside boundaries
  const Acts::TrackingVolume *trackingVolume = trackingGeometry->highestTrackingVolume();
  for (auto volume : trackingVolume->confinedVolumes()->arrayObjects()) {
    for (const auto& layer : volume->confinedLayers()->arrayObjects()) {
      if (layer->layerType() == Acts::LayerType::active) {
        // get inner and outer boundaries from approach descriptor:
        // - innerBoundary is approach suface with minimum z position
        // - outerBoundary is approach suface with maximum z position
        Acts::SurfaceVector approachSurfaces = layer->approachDescriptor()->containedSurfaces();
        const Acts::Surface *innerApproachSurface = *std::min_element(
            approachSurfaces.begin(), approachSurfaces.end(),
            [&gctx](const auto &lhs, const auto &rhs) {
                return lhs->center(gctx).z() < rhs->center(gctx).z();
            }
        );
        const Acts::Surface *outerApproachSurface = *std::max_element(
            approachSurfaces.begin(), approachSurfaces.end(),
            [&gctx](const auto &lhs, const auto &rhs) {
                return lhs->center(gctx).z() < rhs->center(gctx).z();
            }
        );
        double zInnerBoundary = innerApproachSurface->center(gctx).z();
        double zOuterBoundary = outerApproachSurface->center(gctx).z();

        // loop over surface array and check if all edges are between inner and outer boundary
        for (const Acts::Surface *surface : layer->surfaceArray()->surfaces()) {
          auto planeSurface = dynamic_cast<const Acts::PlaneSurface *>(surface);
          // make sure surface has a associated detector element (there are other active detector elements 
          // e.g. containing material or at the tracking volume boundaries)
          if (surface->associatedDetectorElement() != nullptr) {
            auto bounds = dynamic_cast<const Acts::RectangleBounds*>(&surface->bounds());
            const Acts::Vector2 min = bounds->min();
            const Acts::Vector2 max = bounds->max();
            // create dummpy momentum vector: local to global transformation requires  momentum vector,
            // which is ignored for PlaneSurface
            Acts::Vector3 dummyMomentum {1., 1., 1.};
            // get global position at all edges of the surface
            std::vector<Acts::Vector3> edges {};
            edges.push_back(planeSurface->localToGlobal(gctx, {min.x(), min.y()}, dummyMomentum));
            edges.push_back(planeSurface->localToGlobal(gctx, {min.x(), max.y()}, dummyMomentum));
            edges.push_back(planeSurface->localToGlobal(gctx, {max.x(), min.y()}, dummyMomentum));
            edges.push_back(planeSurface->localToGlobal(gctx, {max.x(), max.y()}, dummyMomentum));
            for (const Acts::Vector3 &edgePosition : edges) {
              if ((edgePosition.z() < zInnerBoundary) || (edgePosition.z() > zOuterBoundary)) {
                std::cout << "?? surface outside boundaries\n";
                std::cout << "inner Boundary: " << zInnerBoundary << std::endl;
                std::cout << "outer Boundary: " << zOuterBoundary << std::endl;
                std::cout << "edge: " << edgePosition.x() << ", " << edgePosition.y() << ", " << edgePosition.z() << std::endl;
                return StatusCode::FAILURE;
              }
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserActsGeometryBoundaryTestAlg::finalize() {
  return StatusCode::SUCCESS;
}
