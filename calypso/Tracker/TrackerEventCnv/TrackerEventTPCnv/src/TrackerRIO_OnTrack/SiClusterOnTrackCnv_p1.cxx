/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SiClusterOnTrackCnv_p1.cxx
//
//-----------------------------------------------------------------------------

#include "TrackerRIO_OnTrack/SiClusterOnTrack.h"
#include "TrackerEventTPCnv/TrackerRIO_OnTrack/SiClusterOnTrackCnv_p1.h"
//#include "TrkEventTPCnv/helpers/CLHEPHelpers.h"
//#include "CLHEP/Geometry/Point3D.h"


void SiClusterOnTrackCnv_p1::
persToTrans( const Tracker::SiClusterOnTrack_p1 *persObj,
	     Tracker::SiClusterOnTrack    *transObj,
	     MsgStream &log )
{
   transObj->m_idDE = persObj->m_idDE;
   transObj->m_isbroad = persObj->m_isbroad;
   fillTransFromPStore( &m_RIOCnv, persObj->m_RIO,  transObj, log );
   // m_globalPosition - transient?
}


void SiClusterOnTrackCnv_p1::
transToPers( const Tracker::SiClusterOnTrack    *transObj,
	     Tracker::SiClusterOnTrack_p1 *persObj,
	     MsgStream &log)
{
   persObj->m_RIO = baseToPersistent( &m_RIOCnv, transObj, log );
   persObj->m_idDE = transObj->m_idDE;
   persObj->m_isbroad = transObj->m_isbroad;
  // m_globalPosition - transient?
}
