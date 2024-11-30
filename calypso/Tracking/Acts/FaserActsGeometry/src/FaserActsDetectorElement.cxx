/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsGeometry/FaserActsDetectorElement.h"

// ATHENA
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"

// PACKAGE
// #include "FaserActsTrackingGeometrySvc.h"
#include "FaserActsGeometry/FaserActsAlignmentStore.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "ActsInterop/IdentityHelper.h"

// ACTS
#include "Acts/Surfaces/StrawSurface.hpp"
#include "Acts/Surfaces/LineBounds.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Surfaces/TrapezoidBounds.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Definitions/Units.hpp"

// STL
#include <mutex>

using Acts::Transform3;
using Acts::Surface;

using namespace Acts::UnitLiterals;

constexpr double length_unit = 1_mm;

FaserActsDetectorElement::FaserActsDetectorElement(const TrackerDD::SiDetectorElement* detElem)
{
  m_detElement = detElem;

  //auto center     = detElem->center();
  auto boundsType = detElem->bounds().type();

  // thickness
  m_thickness = detElem->thickness();

  if (boundsType == Trk::SurfaceBounds::Rectangle) {

    const TrackerDD::SiDetectorDesign &design = detElem->design();
    double hlX = design.width()/2. * length_unit;
    double hlY = design.length()/2. * length_unit;

    auto rectangleBounds = std::make_shared<const Acts::RectangleBounds>(hlX, hlY);

    m_bounds = rectangleBounds;

    m_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(rectangleBounds, *this);

  } else if (boundsType == Trk::SurfaceBounds::Trapezoid) {

    std::string shapeString = detElem->getMaterialGeom()->getLogVol()->getShape()->type();

    const TrackerDD::SiDetectorDesign &design = detElem->design();

    double minHlX = design.minWidth()/2. * length_unit;
    double maxHlX = design.maxWidth()/2. * length_unit;
    double hlY    = design.length()/2. * length_unit;

    auto trapezoidBounds = std::make_shared<const Acts::TrapezoidBounds>(
        minHlX, maxHlX, hlY);

    m_bounds = trapezoidBounds;

    m_surface = Acts::Surface::makeShared<Acts::PlaneSurface>(trapezoidBounds, *this);

  } else {
    throw std::domain_error("FaserActsDetectorElement does not support this surface type");
  }
}


IdentityHelper
FaserActsDetectorElement::identityHelper() const
{
    return IdentityHelper(m_detElement);
}

const Acts::Transform3&
FaserActsDetectorElement::transform(const Acts::GeometryContext& anygctx) const
{
  // any cast to known context type
  const FaserActsGeometryContext* gctx = anygctx.get<const FaserActsGeometryContext*>();

  // This is needed for initial geometry construction. At that point, we don't have a
  // consistent view of the geometry yet, and thus we can't populate an alignment store
  // at that time.
  if (gctx->construction) {
    // this should only happen at initialize (1 thread, but mutex anyway)
    return getDefaultTransformMutexed();
  }

  // unpack the alignment store from the context
  const FaserActsAlignmentStore* alignmentStore = gctx->alignmentStore;

  // no GAS, is this initialization?
  assert(alignmentStore != nullptr);

  // get the correct cached transform
  // units should be fine here since we converted at construction
  const Transform3* cachedTrf = alignmentStore->getTransform(this);

  assert(cachedTrf != nullptr);

  return *cachedTrf;
}

void
FaserActsDetectorElement::storeTransform(FaserActsAlignmentStore* gas) const
{
//  Amg::Transform3D g2l
//        = m_detElement->getMaterialGeom()->getDefAbsoluteTransform()
//        * Amg::CLHEPTransformToEigen(m_detElement->recoToHitTransform());
  Amg::Transform3D g2l = m_detElement->transform() ;

  // need to make sure translation has correct units
  g2l.translation() *= length_unit;

  gas->setTransform(this, g2l);
  if (gas->getTransform(this) == nullptr) {
    throw std::runtime_error("Detector element was unable to store transform in GAS");
  }

}

const Acts::Transform3&
FaserActsDetectorElement::getDefaultTransformMutexed() const
{
  Amg::Transform3D g2l
        = m_detElement->getMaterialGeom()->getDefAbsoluteTransform()
        * Amg::CLHEPTransformToEigen(m_detElement->recoToHitTransform());

  // need to make sure translation has correct units
  g2l.translation() *= length_unit;

  std::lock_guard<std::mutex> guard(m_cacheMutex);
  if (m_defTransform) {
    return *m_defTransform;
  }
  // transform not yet set
  m_defTransform
    = std::make_shared<const Acts::Transform3>( g2l );

  return *m_defTransform;
}

const Acts::Surface&
FaserActsDetectorElement::surface() const
{
  return (*m_surface);
}

Acts::Surface&
FaserActsDetectorElement::surface()
{
  return (*const_cast<Acts::Surface*>(&(*m_surface)));
}

double
FaserActsDetectorElement::thickness() const
{
  return m_thickness;
}

Identifier
FaserActsDetectorElement::identify() const
{
  return m_detElement->identify();
}
