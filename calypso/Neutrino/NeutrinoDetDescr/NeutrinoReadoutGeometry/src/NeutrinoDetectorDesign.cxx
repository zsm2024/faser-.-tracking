/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoDetectorDesign.cxx
//   Implementation file for class NeutrinoDetectorDesign
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 3.0 05/05/2001 David Calvet
// Modified: Grant Gorfine
// Modified: Dave Casper
///////////////////////////////////////////////////////////////////

#include "NeutrinoReadoutGeometry/NeutrinoDetectorDesign.h"
#include "NeutrinoReadoutGeometry/NeutrinoIntersect.h"
#include "NeutrinoReadoutGeometry/NeutrinoLocalPosition.h"
#include "TrkSurfaces/RectangleBounds.h"

namespace NeutrinoDD {
// Constructor:
NeutrinoDetectorDesign::NeutrinoDetectorDesign( const double width,
                                          const double height,
                                          const double thickness ) :
    m_thickness(thickness),
    m_width{width},
    m_height{height},
    m_phiAxis(Axis::xAxis),
    m_etaAxis(Axis::yAxis),  
    m_depthAxis(Axis::zAxis)
{
    m_bounds = new Trk::RectangleBounds(0.5*width, 0.5*height);
}


// Destructor:
NeutrinoDetectorDesign::~NeutrinoDetectorDesign() {
    if (m_bounds != nullptr) delete m_bounds;
}

NeutrinoIntersect NeutrinoDetectorDesign::inDetector(const NeutrinoLocalPosition &localPosition,
                                               double xTol, double yTol) const {
    double xDist = 0;
    double yDist = 0;

    distanceToDetectorEdge(localPosition, xDist, yDist);

    NeutrinoIntersect state;

    if (xDist < -xTol || yDist < -yTol) {
        state.setOut();
        return state;
    }

    if (xDist > xTol && yDist > yTol) {
        state.setIn();
        return state;
    }

    // Near boundary.
    state.setNearBoundary();
    return state;
}

DetectorShape NeutrinoDetectorDesign::shape() const {
    // Default is Box.
    return NeutrinoDD::Box;
}

const Trk::SurfaceBounds & 
NeutrinoDetectorDesign::bounds() const
{
  return *m_bounds;
}

// Returns distance to nearest detector edge 
// +ve = inside
// -ve = outside
void
NeutrinoDetectorDesign::distanceToDetectorEdge(const NeutrinoLocalPosition & localPosition,
						   double & etaDist, double & phiDist) const
{ 
  // As the calculation is symmetric around 0,0 we only have to test it for one side.
  double xEta = abs(localPosition.xEta());
  double xPhi = abs(localPosition.xPhi());

  double xEtaEdge = 0.5 * height();
  double xPhiEdge = 0.5 * width();

  // Distance to top/bottom
  etaDist = xEtaEdge - xEta;
  
  // Distance to right/left edge
  phiDist = xPhiEdge - xPhi;

}

} // namespace NeutrinoDD
