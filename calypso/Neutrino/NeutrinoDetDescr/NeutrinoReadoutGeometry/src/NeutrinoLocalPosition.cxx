/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoLocalPosition.cxx
//   Implementation file for class NeutrinoLocalPosition
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 2.1 01/08/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "NeutrinoReadoutGeometry/NeutrinoLocalPosition.h"

namespace NeutrinoDD {

// Default constructor:
NeutrinoLocalPosition::NeutrinoLocalPosition() :
  m_eta(0),
  m_phi(0),
  m_xDepth(0)
{}



// Constructor with parameters:
NeutrinoLocalPosition::NeutrinoLocalPosition(const double x,const double y,
				 const double xDepth) :
  m_eta(x),
  m_phi(y),
  m_xDepth(xDepth)
{}

NeutrinoLocalPosition::NeutrinoLocalPosition(const Amg::Vector2D &position)
  : m_eta(position[Trk::distEta]),
    m_phi(position[Trk::distPhi]),
    m_xDepth(0)
{}
  

NeutrinoLocalPosition::operator Amg::Vector2D(void) const
{
  return Amg::Vector2D(m_phi, m_eta);
}



// addition of positions:
NeutrinoLocalPosition &NeutrinoLocalPosition::operator+=(const NeutrinoLocalPosition &position)
{
  m_eta+=position.m_eta;
  m_phi+=position.m_phi;
  m_xDepth+=position.m_xDepth;  
  return *this;
}

// scaling:
NeutrinoLocalPosition &NeutrinoLocalPosition::operator*=(const double factor)
{
  m_eta*=factor;
  m_phi*=factor;
  m_xDepth*=factor;
  return *this;
}

// scaling:
NeutrinoLocalPosition &NeutrinoLocalPosition::operator/=(const double factor)
{
  if (0!=factor) {
    m_eta/=factor;
    m_phi/=factor;
    m_xDepth/=factor;
  } else {}
  return *this;
}

NeutrinoLocalPosition operator+(const NeutrinoLocalPosition &position1,
			  const NeutrinoLocalPosition &position2)
{
  NeutrinoLocalPosition result(position1);
  result+=position2;
  return result;
}

NeutrinoLocalPosition operator*(const NeutrinoLocalPosition &position,const double factor)
{
  NeutrinoLocalPosition result(position);
  result*=factor;
  return result;
}

NeutrinoLocalPosition operator/(const NeutrinoLocalPosition &position,const double factor)
{
  NeutrinoLocalPosition result(position);
  result/=factor;
  return result;
}

} // namespace NeutrinoDD
