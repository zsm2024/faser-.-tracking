/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloLocalPosition.cxx
//   Implementation file for class CaloLocalPosition
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 2.1 01/08/2001 David Calvet
///////////////////////////////////////////////////////////////////

#include "CaloReadoutGeometry/CaloLocalPosition.h"

namespace CaloDD {

// Default constructor:
CaloLocalPosition::CaloLocalPosition() :
  m_eta(0),
  m_phi(0),
  m_xDepth(0)
{}



// Constructor with parameters:
CaloLocalPosition::CaloLocalPosition(const double x,const double y,
				 const double xDepth) :
  m_eta(x),
  m_phi(y),
  m_xDepth(xDepth)
{}

CaloLocalPosition::CaloLocalPosition(const Amg::Vector2D &position)
  : m_eta(position[Trk::distEta]),
    m_phi(position[Trk::distPhi]),
    m_xDepth(0)
{}
  

CaloLocalPosition::operator Amg::Vector2D(void) const
{
  return Amg::Vector2D(m_phi, m_eta);
}



// addition of positions:
CaloLocalPosition &CaloLocalPosition::operator+=(const CaloLocalPosition &position)
{
  m_eta+=position.m_eta;
  m_phi+=position.m_phi;
  m_xDepth+=position.m_xDepth;  
  return *this;
}

// scaling:
CaloLocalPosition &CaloLocalPosition::operator*=(const double factor)
{
  m_eta*=factor;
  m_phi*=factor;
  m_xDepth*=factor;
  return *this;
}

// scaling:
CaloLocalPosition &CaloLocalPosition::operator/=(const double factor)
{
  if (0!=factor) {
    m_eta/=factor;
    m_phi/=factor;
    m_xDepth/=factor;
  } else {}
  return *this;
}

CaloLocalPosition operator+(const CaloLocalPosition &position1,
			  const CaloLocalPosition &position2)
{
  CaloLocalPosition result(position1);
  result+=position2;
  return result;
}

CaloLocalPosition operator*(const CaloLocalPosition &position,const double factor)
{
  CaloLocalPosition result(position);
  result*=factor;
  return result;
}

CaloLocalPosition operator/(const CaloLocalPosition &position,const double factor)
{
  CaloLocalPosition result(position);
  result/=factor;
  return result;
}

} // namespace CaloDD
