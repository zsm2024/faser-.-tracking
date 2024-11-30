/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloDetectorDesign.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOREADOUTGEOMETRY_CALODETECTORDESIGN_H
#define CALOREADOUTGEOMETRY_CALODETECTORDESIGN_H

// Input/output classes
#include "CaloReadoutGeometry/CaloDD_Defs.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "FaserDetDescr/FaserDetectorID.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/RCBase.h"

#include <list>
#include <vector>


class Identifier;

namespace Trk {
class SurfaceBounds;
class RectangleBounds;
}

namespace CaloDD {
class CaloLocalPosition;
class CaloIntersect;

enum DetectorShape {
  Box=0, Trapezoid, Annulus, Other
};

/** @class CaloDetectorDesign

   Base class for the detector design classes for Ecal.
   These hold the local description of the detector elements which are
   shared by a number of detector elements.

    @author A. Calvet, Grant Gorfine
 */

class CaloDetectorDesign: public RCBase {
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

    CaloDetectorDesign(  const double width,
                         const double height,
                         const double length,
                         const int cells );

    /** Destructor: */
    virtual ~CaloDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    /** Test if point is in the active part of the detector with specified tolerances */
    CaloIntersect inDetector(const CaloLocalPosition &localPosition, double xTol,
                              double yTol) const;

    /** which axis in hit frame is horizontal */
    /** phi corresponds to "width" */
    CaloDetectorDesign::Axis phiAxis() const;

    /** which axis in hit frame is vertical */
    /** eta corresponds to "length" (really "height"...)*/
    CaloDetectorDesign::Axis etaAxis() const;

    /** which axis in hit frame is thickness/beam direction */
    CaloDetectorDesign::Axis depthAxis() const;

    ///////////////////////////////////////////////////////////////////
    // Pure virtual methods:
    ///////////////////////////////////////////////////////////////////

    /** Returns distance to nearest detector active edge
       +ve = inside
       -ve = outside */
    virtual void distanceToDetectorEdge(const CaloLocalPosition &localPosition,
                                        double &xDist, double &yDist) const;

    /** Method to calculate length of a module (global z coordinate)*/
    virtual double length() const;

    /** Method to calculate average width of a module (global x coordinate)*/
    virtual double width() const;

    /** Method which returns thickness of a module (global y coordinate) */
    double height() const;

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
    CaloDetectorDesign();

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:
    double m_width;  
    double m_height; 
    double m_length; 
    int    m_cells;     // !< number of pmts

    CaloDetectorDesign::Axis m_phiAxis;  // which axis in hit frame is horizontal ("width" axis)
    CaloDetectorDesign::Axis m_etaAxis;  // which axis in hit frame is vertical ("height" axis)
    CaloDetectorDesign::Axis m_depthAxis; // which axis in hit frame is depth/beam?

    const Trk::RectangleBounds* m_bounds;

    // Disallow Copy and assignment;
    CaloDetectorDesign(const CaloDetectorDesign &design);
    CaloDetectorDesign &operator = (const CaloDetectorDesign &design);
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline double CaloDetectorDesign::height() const {
    return m_height;
}

inline double CaloDetectorDesign::width() const {
    return m_width;
}

inline double CaloDetectorDesign::length() const {
    return m_length;
}

inline int CaloDetectorDesign::cells() const {
    return m_cells;
}

inline CaloDetectorDesign::Axis CaloDetectorDesign::phiAxis() const {
    return m_phiAxis;
}

inline CaloDetectorDesign::Axis CaloDetectorDesign::etaAxis() const {
    return m_etaAxis;
}

inline CaloDetectorDesign::Axis CaloDetectorDesign::depthAxis() const {
    return m_depthAxis;
}

}  // namespace CaloDD
#endif // CALOREADOUTGEOMETRY_CALODETECTORDESIGN_H
