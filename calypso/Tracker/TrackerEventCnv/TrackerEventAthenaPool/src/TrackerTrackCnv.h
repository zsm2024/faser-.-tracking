/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKER_TRACKCNV_H
#define TRACKER_TRACKCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"

// #include "TrackerEventTPCnv/TrackerTrackCnv_tlp1.h"
#include "TrackerEventTPCnv/TrackerTrackCnv_tlp2.h"

#include "TrackerTrack.h"


//-----------------------------------------------------------------------------
// Base class definition
//-----------------------------------------------------------------------------
typedef Tracker::Track_tlp2	TrackerTrack_PERS;
typedef T_AthenaPoolExtendingCnv<TrackerTrack, TrackerTrack_PERS>       TrackerTrackCnvBase;

//-----------------------------------------------------------------------------
// Converter for TrackCollection object
//-----------------------------------------------------------------------------
class TrackerTrackCnv : public TrackerTrackCnvBase
{
friend class CnvFactory<TrackerTrackCnv>;

protected:
public:
  TrackerTrackCnv( ISvcLocator *svcloc );
protected:

  virtual StatusCode initialize();


  virtual AthenaPoolTopLevelTPCnvBase*	getTopLevelTPCnv() { return &m_TPConverter2; }

  virtual AthenaPoolCnvTPExtension*  clone() { return new TrackerTrackCnv(0); }

  virtual void       readObjectFromPool( const std::string& );

  
private:  
//   InDetTrackCnv_tlp1		m_TPConverter1;
  TrackerTrackCnv_tlp2		m_TPConverter2;
};

#endif

