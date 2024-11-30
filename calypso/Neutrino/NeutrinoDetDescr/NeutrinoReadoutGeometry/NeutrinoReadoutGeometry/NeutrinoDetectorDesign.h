/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoDetectorDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORDESIGN_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORDESIGN_H

// Input/output classes
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"
#include "NeutrinoDD_Defs.h"

#include <list>
#include <vector>


class Identifier;

namespace Trk {
class SurfaceBounds;
class RectangleBounds;
}

namespace NeutrinoDD {
// class SiReadoutCellId;
// class SiCellId;
// class SiDiode;
// class SiReadoutCell;
class NeutrinoLocalPosition;
class NeutrinoIntersect;

enum DetectorShape {
  Box=0, Other
};

/** @class NeutrinoDetectorDesign

   Base class for the detector design classes for Emulsion.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

    @author A. Calvet, Grant Gorfine
 */

class NeutrinoDetectorDesign: public RCBase {
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

    NeutrinoDetectorDesign( const double width,
                            const double height,
                            const double thickness );

    /** Destructor: */
    virtual ~NeutrinoDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    /** Test if point is in the active part of the detector with specified tolerances */
    NeutrinoIntersect inDetector(const NeutrinoLocalPosition &localPosition, double xTol,
                              double yTol) const;

    /** which axis in hit frame is horizontal */
    /** phi corresponds to "width" */
    NeutrinoDetectorDesign::Axis phiAxis() const;

    /** which axis in hit frame is vertical */
    /** eta corresponds to "length" */
    NeutrinoDetectorDesign::Axis etaAxis() const;

    /** which axis in hit frame is thickness/beam direction */
    NeutrinoDetectorDesign::Axis depthAxis() const;

    ///////////////////////////////////////////////////////////////////
    // Pure virtual methods:
    ///////////////////////////////////////////////////////////////////

    /** Returns distance to nearest detector active edge
       +ve = inside
       -ve = outside */
    virtual void distanceToDetectorEdge(const NeutrinoLocalPosition &localPosition,
                                        double &xDist, double &yDist) const;

    /** Method to calculate height of a module */
    virtual double height() const;

    /** Method to calculate average width of a module */
    virtual double width() const;

    /** Method which returns thickness of the silicon wafer */
    double thickness() const;

    // /** Shape of element */
    virtual DetectorShape shape() const;

    /**  Element boundary */
    virtual const Trk::SurfaceBounds &bounds() const;

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    NeutrinoDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    double m_thickness; // !< thickness of plate
    double m_width;     // !< dimension in "phi" direction 
    double m_height;    // !< dimnesion in "eta" direction

    NeutrinoDetectorDesign::Axis m_phiAxis;  // which axis in hit frame is horizontal ("width" axis)
    NeutrinoDetectorDesign::Axis m_etaAxis;  // which axis in hit frame is vertical ("length" axis)
    NeutrinoDetectorDesign::Axis m_depthAxis; // which axis in hit frame is depth/beam?

    const Trk::RectangleBounds* m_bounds;

    // Disallow Copy and assignment;
    NeutrinoDetectorDesign(const NeutrinoDetectorDesign &design);
    NeutrinoDetectorDesign &operator = (const NeutrinoDetectorDesign &design);
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline double NeutrinoDetectorDesign::thickness() const {
    return m_thickness;
}

inline double NeutrinoDetectorDesign::width() const {
    return m_width;
}

inline double NeutrinoDetectorDesign::height() const {
    return m_height;
}

inline NeutrinoDetectorDesign::Axis NeutrinoDetectorDesign::phiAxis() const {
    return m_phiAxis;
}

inline NeutrinoDetectorDesign::Axis NeutrinoDetectorDesign::etaAxis() const {
    return m_etaAxis;
}

inline NeutrinoDetectorDesign::Axis NeutrinoDetectorDesign::depthAxis() const {
    return m_depthAxis;
}

}  // namespace NeutrinoDD
#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINODETECTORDESIGN_H
