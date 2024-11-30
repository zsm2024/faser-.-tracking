/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSDETECTORELEMENT_H
#define FASERACTSGEOMETRY_ACTSDETECTORELEMENT_H

// ATHENA INCLUDES
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

// ACTS
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/GeometryContext.hpp"

// STL
#include <mutex>
#include <iostream>

// BOOST
#include <boost/variant.hpp>

namespace Acts {
  class SurfaceBounds;
}

class IFaserActsTrackingGeometrySvc;
class FaserActsAlignmentStore;

class IdentityHelper;

/// @class FaserActsDetectorElement
///
class FaserActsDetectorElement : public Acts::DetectorElementBase
{
public:
  enum class Subdetector { SCT };

  FaserActsDetectorElement(const TrackerDD::SiDetectorElement* detElem);

  ///  Destructor
  virtual ~FaserActsDetectorElement() {}

  /// Identifier
  Identifier
  identify() const;

  void
  storeTransform(FaserActsAlignmentStore* gas) const;
  virtual const Acts::Transform3 &
  transform(const Acts::GeometryContext &gctx) const final override;


  /// Return surface associated with this identifier, which should come from the
  virtual const Acts::Surface&
  surface() const final override;

  virtual Acts::Surface& surface() override final;


  /// Returns the thickness of the module
  virtual double
  thickness() const final override;

  IdentityHelper identityHelper() const;

 	bool operator==(const FaserActsDetectorElement& otherElem) {
	    return m_detElement->identify() == otherElem.identify();
	}

private:
  
  /// For silicon detectors it is calulated from GM, and stored. Thus the method 
  /// is not const. The store is mutexed.
  const Acts::Transform3&
  getDefaultTransformMutexed() const;

  /// Detector element 
  const TrackerDD::SiDetectorElement* m_detElement;
  /// Boundaries of the detector element
  std::shared_ptr<const Acts::SurfaceBounds> m_bounds;
  ///  Thickness of this detector element
  double m_thickness;
  /// Corresponding Surface
  std::shared_ptr<const Acts::Surface> m_surface;
  std::vector<std::shared_ptr<const Acts::Surface>> m_surfaces;

  // this is pretty much only used single threaded, so
  // the mutex does not hurt
  mutable std::mutex m_cacheMutex;
  mutable std::shared_ptr<const Acts::Transform3> m_defTransform;

  const IFaserActsTrackingGeometrySvc* m_trackingGeometrySvc;
  
  Identifier m_explicitIdentifier;

  // this is threadsafe!
  //mutable Gaudi::Hive::ContextSpecificData<Acts::Transform3> m_ctxSpecificTransform;


};

#endif
