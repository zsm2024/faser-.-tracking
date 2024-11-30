/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoLocalPosition.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINOLOCALPOSITION_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINOLOCALPOSITION_H

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkEventPrimitives/ParamDefs.h"
#include <cmath>

namespace NeutrinoDD {
  enum ExtraLocalPosParam {distDepth = 2}; // These will be defined in Trk soon.
}

namespace NeutrinoDD {

    /** @class NeutrinoLocalPosition
    Class to represent a position in the natural frame of an emulsion sensor
    */

  class NeutrinoLocalPosition {


    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
  public:

    /** Default constructor: */
    NeutrinoLocalPosition();
    
    /** Copy constructor: */
    NeutrinoLocalPosition(const NeutrinoLocalPosition &position) =  default;

    /** This allows one to pass a Amg::Vector2D  to a NeutrinoLocalPosition */
    NeutrinoLocalPosition(const Amg::Vector2D &position);

    /** Constructor with parameters:
        position along eta direction
        position along phi direction
        position along depth direction (default is 0) */
    NeutrinoLocalPosition(const double eta,const double phi,
  		  const double xDepth=0);

    /** Destructor: */
    ~NeutrinoLocalPosition();

    /** Assignment operator: */
    NeutrinoLocalPosition &operator=(const NeutrinoLocalPosition &) = default;
    
    /** Move assignment **/
    NeutrinoLocalPosition &operator=(NeutrinoLocalPosition &&) = default;

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
    NeutrinoLocalPosition &operator+=(const NeutrinoLocalPosition &position);
  
    // so we can go from NeutrinoLocalPosition to Trk::LocalPosition
    operator Amg::Vector2D(void) const;

    // scaling:
    NeutrinoLocalPosition &operator*=(const double factor);
    NeutrinoLocalPosition &operator/=(const double factor);

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
inline NeutrinoLocalPosition::~NeutrinoLocalPosition()
{}

inline double NeutrinoLocalPosition::xEta() const
{
  return m_eta;
}

inline double NeutrinoLocalPosition::xPhi() const
{
  return m_phi;
}

inline double NeutrinoLocalPosition::xDepth() const
{
  return m_xDepth;
}

inline double NeutrinoLocalPosition::r() const
{
  return std::sqrt(m_eta * m_eta + m_phi * m_phi);
}

inline double NeutrinoLocalPosition::phi() const
{
  return std::atan2(m_phi, m_eta);
}

inline void NeutrinoLocalPosition::xEta(const double x)
{
  m_eta=x;
}

inline void NeutrinoLocalPosition::xPhi(const double y)
{
  m_phi=y;
}

inline void NeutrinoLocalPosition::xDepth(const double xDepth)
{
  m_xDepth=xDepth;
}

///////////////////////////////////////////////////////////////////
// Binary operators:
///////////////////////////////////////////////////////////////////
NeutrinoLocalPosition operator+(const NeutrinoLocalPosition &position1,
			  const NeutrinoLocalPosition &position2);

NeutrinoLocalPosition operator*(const NeutrinoLocalPosition &position,const double factor);

inline NeutrinoLocalPosition operator*(const double factor,const NeutrinoLocalPosition &position)
{
  return position*factor;
}

NeutrinoLocalPosition operator/(const NeutrinoLocalPosition &position,const double factor);

} // namespace NeutrinoDD

#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINOLOCALPOSITION_H
