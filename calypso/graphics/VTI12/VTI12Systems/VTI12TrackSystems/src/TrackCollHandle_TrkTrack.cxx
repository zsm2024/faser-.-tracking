/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackCollHandle_TrkTrack          //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12TrackSystems/TrackCollHandle_TrkTrack.h"
#include "VTI12TrackSystems/TrackHandle_TrkTrack.h"
#include "VP1Base/IVP1System.h"
#include "VTI12TrackSystems/TrackSystemController.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1QtUtils.h"
#include "VTI12Utils/VP1JobConfigInfo.h"
#include "VTI12Utils/VP1SGContentsHelper.h"
#include "VTI12Utils/VP1SGAccessHelper.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"

#include <QRegExp>

//____________________________________________________________________
class TrackCollHandle_TrkTrack::Imp {
public:
  static bool isFatrasTruthTrack(const QString& key);
  static bool alwaysShowAllTrackColls;
  TrackCollHandle_TrkTrack * theclass;
  TrackCommonFlags::DETAILLEVEL detailLevel;

};

bool TrackCollHandle_TrkTrack::Imp::alwaysShowAllTrackColls
  = VP1QtUtils::environmentVariableIsOn("VP1_ALWAYS_SHOW_ALL_TRACK_COLLECTIONS");

//____________________________________________________________________
QStringList TrackCollHandle_TrkTrack::availableCollections(IVP1System*sys)
{
  //There must be at least one of the trackers available for this to make sense.
  if ( ! ( VP1JobConfigInfo::hasSCTGeometry() ) ) {
    sys->messageDebug("TrackCollHandle_TrkTrack::availableCollections: " 
      "SCT geometry not available. Won't look in event store for Trk::Track collections");
    return QStringList();
  }

  QStringList keysInSG;
  foreach(QString key,VP1SGContentsHelper(sys).getKeys<TrackCollection>()) {
    // if ( key=="CombinedInDetTracks") // Useful for debugging to limit to one collection
      keysInSG<<key;
  }

  if (keysInSG.isEmpty()||Imp::alwaysShowAllTrackColls)
    return keysInSG;

  QRegExp needsAllIDPattern("ExtendedTracks|ResolvedTracks|Tracks|.*Id.*|.*ID.*|.*InDet.*|.*Inner.*");

  // const bool jobcfgAllID
  //   = VP1JobConfigInfo::hasPixelGeometry()
  //   &&VP1JobConfigInfo::hasSCTGeometry()
  //   &&VP1JobConfigInfo::hasTRTGeometry();
  const bool jobcfgAllID = VP1JobConfigInfo::hasSCTGeometry();

  QStringList outkeys;

  foreach (QString key, keysInSG) {
    if (!jobcfgAllID && needsAllIDPattern.exactMatch(key)) {
      sys->messageDebug("TrackCollHandle_TrkTrack::availableCollections: Ignoring key '"
			+key+"' since sct geometry is not present in job.");
      continue;
    }
    outkeys<<key;
  }
  return outkeys;

}

//____________________________________________________________________
TrackCollHandle_TrkTrack::TrackCollHandle_TrkTrack( TrackSysCommonData * cd,
						    const QString& name,
						    TrackType::Type type )
  : TrackCollHandleBase(cd,name,type), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->detailLevel = TrackCommonFlags::AUTO;
}

//____________________________________________________________________
TrackCollHandle_TrkTrack::~TrackCollHandle_TrkTrack()
{
  delete m_d;
}

//____________________________________________________________________
bool TrackCollHandle_TrkTrack::Imp::isFatrasTruthTrack(const QString& key)
{
  QString env = VP1QtUtils::environmentVariableValue("VP1_FATRAS_TRACKCOLLS_PATTERN");
  return QRegExp((env.isEmpty()?"*fatras*":env),Qt::CaseInsensitive,QRegExp::Wildcard).exactMatch(key);
}

//____________________________________________________________________
void TrackCollHandle_TrkTrack::setupSettingsFromControllerSpecific(TrackSystemController*)
{
  messageVerbose("setupSettingsFromControllerSpecific start");
  messageVerbose("setupSettingsFromControllerSpecific end");
}

//____________________________________________________________________
bool TrackCollHandle_TrkTrack::load()
{
  //Get collection:
  const TrackCollection * trackColl(0);
  if (!VP1SGAccessHelper(systemBase()).retrieve(trackColl, name())) {
    message("Error: Could not retrieve track collection with key="+name());
    return false;
  }

  //Make appropriate trk::track handles:
  hintNumberOfTracksInEvent(trackColl->size());
  int i(0);
  TrackCollection::const_iterator trackItr, trackItrEnd = trackColl->end();
  for ( trackItr = trackColl->begin() ; trackItr != trackItrEnd; ++trackItr) {
    if (*trackItr)
      addTrackHandle(new TrackHandle_TrkTrack(this,*trackItr));
    else
      messageDebug("WARNING: Ignoring null Trk::Track pointer.");
    if (!(i++%100))
      systemBase()->updateGUI();
  }

  //Maybe we need to show measurements, etc.:
  //  updateVisibleAssociatedObjects();

  return true;
}

//____________________________________________________________________
bool TrackCollHandle_TrkTrack::cut(TrackHandleBase* handle)
{
  if (!TrackCollHandleBase::cut(handle))
    return false;

  //fixme: more?
  return true;
}

//____________________________________________________________________
void TrackCollHandle_TrkTrack::visibleStateUpdated(TrackHandle_TrkTrack* trkhandle)
{
  common()->visTrkTracksToMaterialHelper()->setState(trkhandle->trkTrackPointer(), (trkhandle->visible() ? trkhandle->currentMaterial() : 0 ));
}


