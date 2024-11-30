/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1TrackSummary                   //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2009                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12TrackSystems/VP1TrackSummary.h"
#include "VTI12Utils/VP1DetInfo.h"
#include "FaserDetDescr/FaserDetectorID.h"

#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkSegment/Segment.h"

#include <iostream>
//____________________________________________________________________
bool VP1TrackSummary::countHits( const Trk::Track* trk,
    unsigned& nSCTHits
    )
{
    nSCTHits = 0;
    if (!trk)
        return false;

    const FaserDetectorID * idhelper = VP1DetInfo::faserIDHelper();
    if (!idhelper)
        return false;

    std::set<const Trk::TrkDetElementBase*> detelems;// Need several to handle #$$%ing cROTs. EJWM
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_iter = trk->trackStateOnSurfaces()->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsos_end = trk->trackStateOnSurfaces()->end();
    for (; tsos_iter != tsos_end; ++tsos_iter) {
        const Trk::MeasurementBase*       meas = *tsos_iter ? (*tsos_iter)->measurementOnTrack() : 0;
        VP1TrackSummary::addCounts(detelems, meas, idhelper, nSCTHits);
    }
    return true;
}

//____________________________________________________________________
bool VP1TrackSummary::countHits( const Trk::Segment* seg,
    unsigned& nSCTHits
    )
{
    nSCTHits = 0;
    if (!seg)
        return false;

    const FaserDetectorID * idhelper = VP1DetInfo::faserIDHelper();
    if (!idhelper)
        return false;

    std::set<const Trk::TrkDetElementBase*> detelems;// Need several to handle #$$%ing cROTs. EJWM
    std::vector< const Trk::MeasurementBase * >::const_iterator tsos_iter = seg->containedMeasurements().begin(), tsos_end = seg->containedMeasurements().end();
    for (; tsos_iter != tsos_end; ++tsos_iter) {
        const Trk::MeasurementBase*       meas = *tsos_iter;
        addCounts(detelems, meas, idhelper, nSCTHits);
    }
    return true;
}

void VP1TrackSummary::addCounts( std::set<const Trk::TrkDetElementBase*>& detelems, const Trk::MeasurementBase* meas, 
    const FaserDetectorID * idhelper,
    unsigned& nSCTHits
    )
{
    const Trk::CompetingRIOsOnTrack*  crot = dynamic_cast<const Trk::CompetingRIOsOnTrack*>(meas);

    if (crot){
        unsigned int i = 0; 
        for (;i<crot->numberOfContainedROTs();++i){
            detelems.insert(crot->rioOnTrack(i).detectorElement());
        }
    }else if (meas) {
        const Trk::Surface * surf = &(meas->associatedSurface());
        detelems.insert( surf ? surf->associatedDetectorElement() : 0);
    }

    std::set<const Trk::TrkDetElementBase*>::const_iterator it = detelems.begin(), itEnd=detelems.end();
    for (;it!=itEnd;++it){
        const Trk::TrkDetElementBase* detelem=*it;
        if (!detelem) continue;
        Identifier id = detelem->identify();
        if (!id.is_valid())
            continue;
        if (idhelper->is_sct(id)) ++nSCTHits;
    }
    detelems.clear();   
}
