/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloLocalPosition.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOREADOUTGEOMETRY_CALOLOCALPOSITION_H
#define CALOREADOUTGEOMETRY_CALOLOCALPOSITION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <cmath>

namespace CaloDD {
  enum ExtraLocalPosParam {distDepth = 2}; // These will be defined in Trk soon.
}

namespace CaloDD {

    /** @class CaloLocalPosition
    Class to represent a position in the natural frame of a calorimeter sensor
    */

  class CaloLocalPosition {


    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /** Default constructor: */
    CaloLocalPosition();
    
    /** Copy constructor: */
    CaloLocalPosition(const CaloLocalPosition &position) =  default;

    /** This allows one to pass a Amg::Vector2D  to a CaloLocalPosition */
    CaloLocalPosition(const Amg::Vector2D &position);

    /** Constructor with parameters:
        position along eta direction
        position along phi direction
        position along depth direction (default is 0) */
    CaloLocalPosition(const double eta,const double phi,
  		  const double xDepth=0);

    /** Destructor: */
    ~CaloLocalPosition();

    /** Assignment operator: */
    CaloLocalPosition &operator=(const CaloLocalPosition &) = default;
    
    /** Move assignment **/
    CaloLocalPosition &operator=(CaloLocalPosition &&) = default;

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
    CaloLocalPosition &operator+=(const CaloLocalPosition &position);
  
    // so we can go from CaloLocalPosition to Trk::LocalPosition
    operator Amg::Vector2D(void) const;

    // scaling:
    CaloLocalPosition &operator*=(const double factor);
    CaloLocalPosition &operator/=(const double factor);

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
inline CaloLocalPosition::~CaloLocalPosition()
{}

inline double CaloLocalPosition::xEta() const
{
  return m_eta;
}

inline double CaloLocalPosition::xPhi() const
{
  return m_phi;
}

inline double CaloLocalPosition::xDepth() const
{
  return m_xDepth;
}

inline double CaloLocalPosition::r() const
{
  return std::sqrt(m_eta * m_eta + m_phi * m_phi);
}

inline double CaloLocalPosition::phi() const
{
  return std::atan2(m_phi, m_eta);
}

inline void CaloLocalPosition::xEta(const double y)
{
  m_eta=y;
}

inline void CaloLocalPosition::xPhi(const double x)
{
  m_phi=x;
}

inline void CaloLocalPosition::xDepth(const double xDepth)
{
  m_xDepth=xDepth;
}

///////////////////////////////////////////////////////////////////
// Binary operators:
///////////////////////////////////////////////////////////////////
CaloLocalPosition operator+(const CaloLocalPosition &position1,
			  const CaloLocalPosition &position2);

CaloLocalPosition operator*(const CaloLocalPosition &position,const double factor);

inline CaloLocalPosition operator*(const double factor,const CaloLocalPosition &position)
{
  return position*factor;
}

CaloLocalPosition operator/(const CaloLocalPosition &position,const double factor);

} // namespace CaloDD

#endif // CALOREADOUTGEOMETRY_CALOLOCALPOSITION_H
