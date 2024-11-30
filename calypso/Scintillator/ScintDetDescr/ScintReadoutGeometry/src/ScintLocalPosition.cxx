/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintLocalPosition.cxx
//   Implementation file for class ScintLocalPosition
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 2.1 01/08/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "ScintReadoutGeometry/ScintLocalPosition.h"

namespace ScintDD {

// Default constructor:
ScintLocalPosition::ScintLocalPosition() :
  m_eta(0),
  m_phi(0),
  m_xDepth(0)
{}



// Constructor with parameters:
ScintLocalPosition::ScintLocalPosition(const double x,const double y,
				 const double xDepth) :
  m_eta(x),
  m_phi(y),
  m_xDepth(xDepth)
{}

ScintLocalPosition::ScintLocalPosition(const Amg::Vector2D &position)
  : m_eta(position[Trk::distEta]),
    m_phi(position[Trk::distPhi]),
    m_xDepth(0)
{}
  

ScintLocalPosition::operator Amg::Vector2D(void) const
{
  return Amg::Vector2D(m_phi, m_eta);
}



// addition of positions:
ScintLocalPosition &ScintLocalPosition::operator+=(const ScintLocalPosition &position)
{
  m_eta+=position.m_eta;
  m_phi+=position.m_phi;
  m_xDepth+=position.m_xDepth;  
  return *this;
}

// scaling:
ScintLocalPosition &ScintLocalPosition::operator*=(const double factor)
{
  m_eta*=factor;
  m_phi*=factor;
  m_xDepth*=factor;
  return *this;
}

// scaling:
ScintLocalPosition &ScintLocalPosition::operator/=(const double factor)
{
  if (0!=factor) {
    m_eta/=factor;
    m_phi/=factor;
    m_xDepth/=factor;
  } else {}
  return *this;
}

ScintLocalPosition operator+(const ScintLocalPosition &position1,
			  const ScintLocalPosition &position2)
{
  ScintLocalPosition result(position1);
  result+=position2;
  return result;
}

ScintLocalPosition operator*(const ScintLocalPosition &position,const double factor)
{
  ScintLocalPosition result(position);
  result*=factor;
  return result;
}

ScintLocalPosition operator/(const ScintLocalPosition &position,const double factor)
{
  ScintLocalPosition result(position);
  result/=factor;
  return result;
}

} // namespace ScintDD
