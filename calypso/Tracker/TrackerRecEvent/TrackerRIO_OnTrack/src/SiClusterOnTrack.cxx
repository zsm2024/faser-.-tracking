/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS and FASER collaborations
*/

///////////////////////////////////////////////////////////////////
// SiClusterOnTrack.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrackerRIO_OnTrack/SiClusterOnTrack.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkSurfaces/Surface.h"
#include "TrackerPrepRawData/TrackerCluster.h"
#include "GaudiKernel/MsgStream.h"
#include <memory>
#include <new>
#include <ostream>
#include <typeinfo>


// Constructor with parameters - global position not specified here
Tracker::SiClusterOnTrack::SiClusterOnTrack( Trk::LocalParameters&& locpars, 
                                             Amg::MatrixX&& locerr, 
                                             const IdentifierHash& idDE,
                                             const Identifier& id,
                                             bool isbroad) : 
  RIO_OnTrack(std::move(locpars), std::move(locerr), id), //call base class constructor
  m_idDE(idDE),
  m_globalPosition(), // should be set in constructor of derived class
  m_isbroad(isbroad)
{}

// Constructor with parameters - global position specified
Tracker::SiClusterOnTrack::SiClusterOnTrack( Trk::LocalParameters&& locpars,
                                             Amg::MatrixX&& locerr,
                                            const IdentifierHash& idDE,
                                            const Identifier& id,
                                            const Amg::Vector3D& globalPosition,
                                            bool isbroad) 
    : 
    RIO_OnTrack(std::move(locpars), std::move(locerr), id), //call base class constructor
    m_idDE(idDE),
    m_globalPosition(globalPosition),
    m_isbroad(isbroad)
{}


// Destructor:
Tracker::SiClusterOnTrack::~SiClusterOnTrack()
{ 
}

// Default constructor:
Tracker::SiClusterOnTrack::SiClusterOnTrack():
    Trk::RIO_OnTrack(),
    m_idDE(),
    m_globalPosition(), // should be set in constructor of derived class
    m_isbroad(false)
{}

// copy constructor:
Tracker::SiClusterOnTrack::SiClusterOnTrack( const SiClusterOnTrack& rot)
    :
    RIO_OnTrack(rot),
    m_idDE(rot.m_idDE),
    m_globalPosition(rot.m_globalPosition),
    m_isbroad(rot.m_isbroad)
{}

// assignment operator:
Tracker::SiClusterOnTrack& Tracker::SiClusterOnTrack::operator=( const SiClusterOnTrack& rot){
    if ( &rot != this) {
       Trk::RIO_OnTrack::operator=(rot);
       m_idDE           = rot.m_idDE;
       m_globalPosition = rot.m_globalPosition;
       m_isbroad        = rot.m_isbroad;
    }
    return *this;
}




const Amg::Vector3D& Tracker::SiClusterOnTrack::globalPosition() const
{ 
  return m_globalPosition;
}

MsgStream& Tracker::SiClusterOnTrack::dump( MsgStream& sl ) const
{

    sl << "SiClusterOnTrack {" << endmsg;
    Trk::RIO_OnTrack::dump(sl);

    sl << "Global position (x,y,z) = (";
    sl  <<this->globalPosition().x()<<", "
        <<this->globalPosition().y()<<", "
        <<this->globalPosition().z()<<")"<<endmsg;
    sl<<"}"<<endmsg;
    return sl;
}

std::ostream& Tracker::SiClusterOnTrack::dump( std::ostream& sl ) const
{
    sl << "SiClusterOnTrack {"<<std::endl;

    Trk::RIO_OnTrack::dump(sl);

    sl << "Global position (x,y,z) = (";
    sl  <<this->globalPosition().x()<<", "
        <<this->globalPosition().y()<<", "
        <<this->globalPosition().z()<<")"<<std::endl;
    sl<<"}"<<std::endl;
    return sl;
}






