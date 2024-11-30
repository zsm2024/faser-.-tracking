/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations
*/

///////////////////////////////////////////////////////////////////
// FaserSCT_ClusterOnTrack.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

#include "TrackerRIO_OnTrack/FaserSCT_ClusterOnTrack.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"

#include "GaudiKernel/MsgStream.h"
#include <ostream>
#include <limits>


// Constructor with parameters - no global position assigned
Tracker::FaserSCT_ClusterOnTrack::FaserSCT_ClusterOnTrack(const Tracker::FaserSCT_Cluster* RIO, 
           Trk::LocalParameters&& locpars, 
           Amg::MatrixX&& locerr, 
           const IdentifierHash& idDE,
           bool isbroad) : 
  SiClusterOnTrack(std::move(locpars), std::move(locerr), idDE, RIO->identify(),isbroad) //call base class constructor
{
  m_detEl=RIO->detectorElement();
  m_positionAlongStrip=std::numeric_limits<double>::quiet_NaN();
  m_rio.setElement(RIO);

  // Set global position
  Amg::Vector2D lpos(localParameters().get(Trk::locX), m_positionAlongStrip);
  m_globalPosition = detectorElement()->surface(identify()).localToGlobal(lpos);
}

// Constructor with parameters
Tracker::FaserSCT_ClusterOnTrack::FaserSCT_ClusterOnTrack(const Tracker::FaserSCT_Cluster* RIO, 
				   Trk::LocalParameters&& locpars,
				   Amg::MatrixX&& locerr, 
				   const IdentifierHash& idDE,
				   const Amg::Vector3D& globalPosition,
                                   bool isbroad) : 
  SiClusterOnTrack(std::move(locpars), std::move(locerr), idDE, RIO->identify(), globalPosition, isbroad), //call base class constructor
  m_detEl( RIO->detectorElement() )
{
  m_rio.setElement(RIO);

  //constructing local position provided a global one  
  std::optional<Amg::Vector2D> lpos{
    detectorElement()->surface(identify()).positionOnSurface(globalPosition)
  };
					
  //storing the position along the strip if available
  m_positionAlongStrip = (lpos) ? (*lpos)[Trk::locY]:0.; 

}

Tracker::FaserSCT_ClusterOnTrack::FaserSCT_ClusterOnTrack( const ElementLinkToTrackerCFaserSCT_ClusterContainer& RIO,
                                                           const Trk::LocalParameters& locpars, 
                                                           const Amg::MatrixX& locerr, 
                                                           IdentifierHash idDE,
                                                           const Identifier& id,
                                                           bool isbroad,
                                                           double positionAlongStrip)
  : SiClusterOnTrack(Trk::LocalParameters(locpars),
                     Amg::MatrixX(locerr),
                     idDE,
                     id,
                     isbroad),
    m_rio(RIO),
    m_detEl(nullptr),
    m_positionAlongStrip (positionAlongStrip)
{
}


// Default constructor:
Tracker::FaserSCT_ClusterOnTrack::FaserSCT_ClusterOnTrack():
  SiClusterOnTrack(),
  m_rio(),
  m_detEl(nullptr),
  m_positionAlongStrip(std::numeric_limits<double>::quiet_NaN())
{}


const Trk::Surface& Tracker::FaserSCT_ClusterOnTrack::associatedSurface() const
{ 
    return ( detectorElement()->surface()); 
}
  
void Tracker::FaserSCT_ClusterOnTrack::setValues(const Trk::TrkDetElementBase* detEl, const Trk::PrepRawData* prepRawData)
{
    const auto *cluster = dynamic_cast<const Tracker::FaserSCT_Cluster*>(prepRawData);
    m_rio.setElement(cluster);
    m_detEl = dynamic_cast< const TrackerDD::SiDetectorElement* >(detEl);
    if (m_detEl) {
      // Set global position after setting the detector element
      Amg::Vector2D lpos(localParameters().get(Trk::locX),
                         m_positionAlongStrip);
      m_globalPosition =
        detectorElement()->surface(identify()).localToGlobal(lpos);
    }
}

MsgStream& Tracker::FaserSCT_ClusterOnTrack::dump( MsgStream& sl ) const
{
  SiClusterOnTrack::dump(sl);// use dump(...) from SiClusterOnTrack
  return sl;
}

std::ostream& Tracker::FaserSCT_ClusterOnTrack::dump( std::ostream& sl ) const
{
  SiClusterOnTrack::dump(sl);// use dump(...) from SiClusterOnTrack
  return sl;
}
