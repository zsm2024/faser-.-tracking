/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintDetectorDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCINTREADOUTGEOMETRY_SCINTDETECTORDESIGN_H
#define SCINTREADOUTGEOMETRY_SCINTDETECTORDESIGN_H

// Input/output classes
#include "ScintReadoutGeometry/ScintDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"
#include "ScintDD_Defs.h"

#include <list>
#include <vector>


class Identifier;

namespace Trk {
class SurfaceBounds;
class RectangleBounds;
}

namespace ScintDD {
// class SiReadoutCellId;
// class SiCellId;
// class SiDiode;
// class SiReadoutCell;
class ScintLocalPosition;
class ScintIntersect;

enum DetectorShape {
  Box=0, Trapezoid, Annulus, Other
};

/** @class ScintDetectorDesign

   Base class for the detector design classes for Veto, Trigger and Preshower.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

    @author A. Calvet, Grant Gorfine
 */

class ScintDetectorDesign: public RCBase {
public:
    enum Axis {
        xAxis = 0, yAxis, zAxis
    };

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:
    /** Constructor
     */

    ScintDetectorDesign( const double thickness,
                         const double width,
                         const double length,
                         const int cells );

    /** Destructor: */
    virtual ~ScintDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    /** Test if point is in the active part of the detector with specified tolerances */
    ScintIntersect inDetector(const ScintLocalPosition &localPosition, double xTol,
                              double yTol) const;

    /** which axis in hit frame is horizontal */
    /** phi corresponds to "width" */
    ScintDetectorDesign::Axis phiAxis() const;

    /** which axis in hit frame is vertical */
    /** eta corresponds to "length" */
    ScintDetectorDesign::Axis etaAxis() const;

    /** which axis in hit frame is thickness/beam direction */
    ScintDetectorDesign::Axis depthAxis() const;

    ///////////////////////////////////////////////////////////////////
    // Pure virtual methods:
    ///////////////////////////////////////////////////////////////////

    /** Returns distance to nearest detector active edge
       +ve = inside
       -ve = outside */
    virtual void distanceToDetectorEdge(const ScintLocalPosition &localPosition,
                                        double &xDist, double &yDist) const;

    /** Method to calculate length of a module */
    virtual double length() const;

    /** Method to calculate average width of a module */
    virtual double width() const;

    /** Method which returns thickness of the silicon wafer */
    double thickness() const;

    /** Number of PMTs */
    int cells() const;

    // /** Shape of element */
    virtual DetectorShape shape() const;

    /**  Element boundary */
    virtual const Trk::SurfaceBounds &bounds() const;

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    ScintDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    double m_thickness; // !< thickness of plate
    double m_width;     // !< dimension in "phi" direction 
    double m_length;    // !< dimnesion in "eta" direction
    int    m_cells;     // !< number of pmts

    ScintDetectorDesign::Axis m_phiAxis;  // which axis in hit frame is horizontal ("width" axis)
    ScintDetectorDesign::Axis m_etaAxis;  // which axis in hit frame is vertical ("length" axis)
    ScintDetectorDesign::Axis m_depthAxis; // which axis in hit frame is depth/beam?

    const Trk::RectangleBounds* m_bounds;

    // Disallow Copy and assignment;
    ScintDetectorDesign(const ScintDetectorDesign &design);
    ScintDetectorDesign &operator = (const ScintDetectorDesign &design);
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline double ScintDetectorDesign::thickness() const {
    return m_thickness;
}

inline double ScintDetectorDesign::width() const {
    return m_width;
}

inline double ScintDetectorDesign::length() const {
    return m_length;
}

inline int ScintDetectorDesign::cells() const {
    return m_cells;
}

inline ScintDetectorDesign::Axis ScintDetectorDesign::phiAxis() const {
    return m_phiAxis;
}

inline ScintDetectorDesign::Axis ScintDetectorDesign::etaAxis() const {
    return m_etaAxis;
}

inline ScintDetectorDesign::Axis ScintDetectorDesign::depthAxis() const {
    return m_depthAxis;
}

}  // namespace ScintDD
#endif // SCINTREADOUTGEOMETRY_SCINTDETECTORDESIGN_H
