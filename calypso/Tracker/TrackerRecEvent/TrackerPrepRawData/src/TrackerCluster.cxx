/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// TrackerCluster.cxx
//   Implementation file for class TrackerCluster
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 15/07/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrackerPrepRawData/TrackerCluster.h"
#include "GaudiKernel/MsgStream.h"

namespace Tracker {
// Constructor for EF:
TrackerCluster::TrackerCluster(const Identifier& RDOId,
                    const Amg::Vector2D& locpos,
                    const std::vector<Identifier>& rdoList,
                    const FaserSiWidth& width,
                    const TrackerDD::SiDetectorElement* detEl,
                    const Amg::MatrixX& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, locErrMat)
  , m_width(width)
  , m_detEl(detEl)
{
  if (m_detEl) {
    m_globalPosition =
      m_detEl->surface(identify()).localToGlobal(localPosition());
  }
}

// Constructor for EF:
TrackerCluster::TrackerCluster(const Identifier& RDOId,
                    const Amg::Vector2D& locpos,
                    const std::vector<Identifier>& rdoList,
                    const FaserSiWidth& width,
                    const TrackerDD::SiDetectorElement* detEl)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, {})
  , m_width(width)
  , m_detEl(detEl)
{
  if (m_detEl) {
    m_globalPosition =
      m_detEl->surface(identify()).localToGlobal(localPosition());
  }
}

TrackerCluster::TrackerCluster(const Identifier& RDOId,
                    const Amg::Vector2D& locpos,
                    const Amg::Vector3D& globpos,
                    const std::vector<Identifier>& rdoList,
                    const FaserSiWidth& width,
                    const TrackerDD::SiDetectorElement* detEl,
                    const Amg::MatrixX& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList, locErrMat)
  , m_globalPosition(globpos)
  , m_width(width)
  , m_detEl(detEl)
{}

TrackerCluster::TrackerCluster(const Identifier& RDOId,
                    const Amg::Vector2D& locpos,
                    const Amg::Vector3D& globpos,
                    const std::vector<Identifier>& rdoList,
                    const FaserSiWidth& width,
                    const TrackerDD::SiDetectorElement* detEl)
  : // call base class constructor
  PrepRawData(RDOId, locpos, rdoList,{})
  , m_globalPosition(globpos)
  , m_width(width)
  , m_detEl(detEl)
{}

TrackerCluster::TrackerCluster(const Identifier& RDOId,
                    const Amg::Vector2D& locpos,
                    std::vector<Identifier>&& rdoList,
                    const FaserSiWidth& width,
                    const TrackerDD::SiDetectorElement* detEl,
                    Amg::MatrixX&& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, std::move(rdoList), std::move(locErrMat))
  , m_width(width)
  , m_detEl(detEl)
{
  if (m_detEl) {
    m_globalPosition =
      m_detEl->surface(identify()).localToGlobal(localPosition());
  }
}

TrackerCluster::TrackerCluster(const Identifier& RDOId,
                    const Amg::Vector2D& locpos,
                    const Amg::Vector3D& globpos,
                    std::vector<Identifier>&& rdoList,
                    const FaserSiWidth& width,
                    const TrackerDD::SiDetectorElement* detEl,
                    Amg::MatrixX&& locErrMat)
  : // call base class constructor
  PrepRawData(RDOId, locpos, std::move(rdoList), std::move(locErrMat))
  , m_globalPosition(globpos)
  , m_width(width)
  , m_detEl(detEl)
{}


    // Destructor:
TrackerCluster::~TrackerCluster()
{
  // do not delete m_detEl since owned by DetectorStore
}

MsgStream& TrackerCluster::dump( MsgStream&    stream) const
{
  stream << "TrackerCluster object"<<std::endl;

  // have to do a lot of annoying checking to make sure that PRD is valid. 
  {
    stream << "at global coordinates (x,y,z) = ("<<this->globalPosition().x()<<", "
      <<this->globalPosition().y()<<", "
      <<this->globalPosition().z()<<")"<<std::endl;
  }

  stream << "FaserSiWidth: " << m_width << std::endl;

  stream << "Base class (PrepRawData):" << std::endl;
  this->PrepRawData::dump(stream);

  return stream;
}

std::ostream& TrackerCluster::dump( std::ostream&    stream) const
{
  stream << "TrackerCluster object"<<std::endl;
  {
    stream << "at global coordinates (x,y,z) = ("<<this->globalPosition().x()<<", "
      <<this->globalPosition().y()<<", "
      <<this->globalPosition().z()<<")"<<std::endl;
  }

  stream << "FaserSiWidth: " << m_width << std::endl;

  stream << "Base Class (PrepRawData): " << std::endl;
  this->PrepRawData::dump(stream);

  return stream;
}


MsgStream&    operator << (MsgStream& stream,    const TrackerCluster& prd)
{
  return prd.dump(stream);
}

std::ostream& operator << (std::ostream& stream, const TrackerCluster& prd)
{
  return prd.dump(stream);
}

}
