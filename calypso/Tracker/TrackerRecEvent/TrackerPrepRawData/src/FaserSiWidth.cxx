/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSiWidth.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackerPrepRawData/FaserSiWidth.h"
#include "GaudiKernel/MsgStream.h"

namespace Tracker {
// Implicit constructor:
FaserSiWidth::FaserSiWidth() :
  m_colrow(0.0, 0.0),
  m_phirzWidth(0.0, 0.0)
{}

// Copy constructor:
FaserSiWidth::FaserSiWidth(const FaserSiWidth& width) :
  m_colrow(width.m_colrow),
  m_phirzWidth(width.m_phirzWidth)
{}

// Constructor with parameters:
FaserSiWidth::FaserSiWidth(const Amg::Vector2D& colrow, const  Amg::Vector2D& phiRZ) :
  m_colrow( colrow ),
  m_phirzWidth(phiRZ)
{}


// online constructor:
FaserSiWidth::FaserSiWidth(const Amg::Vector2D& colrow) :
  m_colrow(colrow),
  m_phirzWidth(0,0)
{}


// Destructor:
FaserSiWidth::~FaserSiWidth()
{
}

// Assignment operator:
FaserSiWidth& FaserSiWidth::operator=(const FaserSiWidth &width)
{
  if (this!=&width) {
    m_colrow = width.m_colrow;
    m_phirzWidth = width.m_phirzWidth;
  } 
  return *this;
}

// addition of positions:


// scaling:

// Dump methods
MsgStream& FaserSiWidth::dump( MsgStream&    stream) const
{
  stream << "FaserSiWidth object"<<std::endl;
  stream << "Column / Row " << m_colrow.x()     << " " << m_colrow.y() << std::endl;
  stream << "phi RZ width " << m_phirzWidth.x() << " " << m_phirzWidth.y() << std::endl;
  return stream;
}
std::ostream& FaserSiWidth::dump( std::ostream&    stream) const
{
  stream << "FaserSiWidth object"<<std::endl;
  stream <<"Column / Row " << m_colrow.x() << " " << m_colrow.y() << std::endl;
  stream <<"phi RZ width " << m_phirzWidth.x() << " " << m_phirzWidth.y() << std::endl;
  return stream;
}

MsgStream&    operator << (MsgStream& stream,    const FaserSiWidth& prd)
{
  return prd.dump(stream);
}

std::ostream& operator << (std::ostream& stream, const FaserSiWidth& prd)
{
  return prd.dump(stream);
}

}