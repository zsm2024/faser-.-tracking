/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintLocalPosition.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCINTREADOUTGEOMETRY_SCINTLOCALPOSITION_H
#define SCINTREADOUTGEOMETRY_SCINTLOCALPOSITION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <cmath>

namespace ScintDD {
  enum ExtraLocalPosParam {distDepth = 2}; // These will be defined in Trk soon.
}

namespace ScintDD {

    /** @class ScintLocalPosition
    Class to represent a position in the natural frame of a scintillator sensor, for Veto, Trigger and Preshower
    */

  class ScintLocalPosition {


    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /** Default constructor: */
    ScintLocalPosition();
    
    /** Copy constructor: */
    ScintLocalPosition(const ScintLocalPosition &position) =  default;

    /** This allows one to pass a Amg::Vector2D  to a ScintLocalPosition */
    ScintLocalPosition(const Amg::Vector2D &position);

    /** Constructor with parameters:
        position along eta direction
        position along phi direction
        position along depth direction (default is 0) */
    ScintLocalPosition(const double eta,const double phi,
  		  const double xDepth=0);

    /** Destructor: */
    ~ScintLocalPosition();

    /** Assignment operator: */
    ScintLocalPosition &operator=(const ScintLocalPosition &) = default;
    
    /** Move assignment **/
    ScintLocalPosition &operator=(ScintLocalPosition &&) = default;

    ///////////////////////////////////////////////////////////////////
    // Const methods:
    ///////////////////////////////////////////////////////////////////

    /** position along local eta direction:*/
    double xEta() const;

    /** position along local phi direction:*/
    double xPhi() const;

    /** Cylindrical coordinate r:*/
    double r() const;

    /** Cylindrical coordinate phi:*/
    double phi() const;

    /** position along depth direction: */
    double xDepth() const;

    ///////////////////////////////////////////////////////////////////
    // Non-const methods:
    ///////////////////////////////////////////////////////////////////

    void xEta(const double eta);

    void xPhi(const double phi);

    void xDepth(const double xDepth);

    // addition of positions:
    ScintLocalPosition &operator+=(const ScintLocalPosition &position);
  
    // so we can go from ScintLocalPosition to Trk::LocalPosition
    operator Amg::Vector2D(void) const;

    // scaling:
    ScintLocalPosition &operator*=(const double factor);
    ScintLocalPosition &operator/=(const double factor);

    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////

  private:
    double m_eta; //!< position along eta direction
    double m_phi; //!< position along phi direction
    double m_xDepth; //!< position along depth direction
  };

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
inline ScintLocalPosition::~ScintLocalPosition()
{}

inline double ScintLocalPosition::xEta() const
{
  return m_eta;
}

inline double ScintLocalPosition::xPhi() const
{
  return m_phi;
}

inline double ScintLocalPosition::xDepth() const
{
  return m_xDepth;
}

inline double ScintLocalPosition::r() const
{
  return std::sqrt(m_eta * m_eta + m_phi * m_phi);
}

inline double ScintLocalPosition::phi() const
{
  return std::atan2(m_phi, m_eta);
}

inline void ScintLocalPosition::xEta(const double x)
{
  m_eta=x;
}

inline void ScintLocalPosition::xPhi(const double y)
{
  m_phi=y;
}

inline void ScintLocalPosition::xDepth(const double xDepth)
{
  m_xDepth=xDepth;
}

///////////////////////////////////////////////////////////////////
// Binary operators:
///////////////////////////////////////////////////////////////////
ScintLocalPosition operator+(const ScintLocalPosition &position1,
			  const ScintLocalPosition &position2);

ScintLocalPosition operator*(const ScintLocalPosition &position,const double factor);

inline ScintLocalPosition operator*(const double factor,const ScintLocalPosition &position)
{
  return position*factor;
}

ScintLocalPosition operator/(const ScintLocalPosition &position,const double factor);

} // namespace ScintDD

#endif // SCINTREADOUTGEOMETRY_SCINTLOCALPOSITION_H
