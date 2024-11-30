/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

///////////////////////////////////////////////////////////////////
// FaserSCT_Cluster.cxx
//   Implementation file for class FaserSCT_Cluster
///////////////////////////////////////////////////////////////////

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
// forward includes
#include "GaudiKernel/MsgStream.h"
#include "TrackerPrepRawData/FaserSiWidth.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

namespace Tracker {

FaserSCT_Cluster::FaserSCT_Cluster( 
    const Identifier& RDOId,
    const Amg::Vector2D& locpos, 
    const std::vector<Identifier>& rdoList,
    const FaserSiWidth& width,
    const TrackerDD::SiDetectorElement* detEl,
    const Amg::MatrixX& locErrMat
    ) : TrackerCluster(RDOId, locpos, rdoList, width, detEl, locErrMat) 
{
  m_hitsInThirdTimeBin=0;
}

FaserSCT_Cluster::FaserSCT_Cluster(const Identifier& RDOId,
                         const Amg::Vector2D& locpos,
                         std::vector<Identifier>&& rdoList,
                         const FaserSiWidth& width,
                         const TrackerDD::SiDetectorElement* detEl,
                         Amg::MatrixX&& locErrMat)
  : TrackerCluster(RDOId,
              locpos,
              std::move(rdoList),
              width,
              detEl,
              std::move(locErrMat))
  , m_hitsInThirdTimeBin(0)
{}


// Default constructor:
FaserSCT_Cluster::FaserSCT_Cluster():TrackerCluster()
{
  m_hitsInThirdTimeBin=0;
}

//copy constructor:
FaserSCT_Cluster::FaserSCT_Cluster(const FaserSCT_Cluster& RIO):
  TrackerCluster(RIO)
{
  m_hitsInThirdTimeBin = RIO.hitsInThirdTimeBin();
}

//move constructor:
FaserSCT_Cluster::FaserSCT_Cluster(FaserSCT_Cluster&& RIO):
  TrackerCluster(std::move(RIO)),
  m_hitsInThirdTimeBin (RIO.m_hitsInThirdTimeBin)
{
}

//assignment operator
FaserSCT_Cluster& FaserSCT_Cluster::operator=(const FaserSCT_Cluster& RIO){
  if(&RIO != this) {
    TrackerCluster::operator=(RIO);
    m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
  }
  return *this;
}

//move operator
FaserSCT_Cluster& FaserSCT_Cluster::operator=(FaserSCT_Cluster&& RIO) {
  if(&RIO != this) {
    TrackerCluster::operator=(std::move(RIO));
    m_hitsInThirdTimeBin = RIO.m_hitsInThirdTimeBin;
  }
  return *this;
}


MsgStream&    operator << (MsgStream& stream,    const FaserSCT_Cluster& prd)
{
  return prd.dump(stream);
}

std::ostream& operator << (std::ostream& stream, const FaserSCT_Cluster& prd)
{
  return prd.dump(stream);
}

MsgStream& FaserSCT_Cluster::dump( MsgStream&    stream) const
{
  stream << "FaserSCT_Cluster object"<<std::endl;
  stream <<  "Base class (TrackerCluster):" << std::endl;
  this->TrackerCluster::dump(stream);

  return stream;
}

std::ostream& FaserSCT_Cluster::dump( std::ostream&    stream) const
{
  stream << "FaserSCT_Cluster object"<<std::endl;
  stream <<  "Base class (TrackerCluster):" << std::endl;
  this->TrackerCluster::dump(stream);

  return stream;
}


}