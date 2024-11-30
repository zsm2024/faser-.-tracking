/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include "TruthSeededTrackFinderTool/TruthSeededTrackFinderTool.h"

// Cluster and space point collections
#include "TrkSpacePoint/SpacePointCollection.h"
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"

namespace Tracker
{

static const InterfaceID IID_ITruthSeededTrackFinderTool
("TruthSeededTrackFinderTool", 99490493, 0);

const InterfaceID& TruthSeededTrackFinderTool::interfaceID() { 
  return IID_ITruthSeededTrackFinderTool; 
}

// Constructor with parameters:
TruthSeededTrackFinderTool::TruthSeededTrackFinderTool(const std::string& type,
    const std::string& name,
    const IInterface* parent) :
  AthAlgTool(type, name, parent) {
    declareInterface< TruthSeededTrackFinderTool>(this);
  }
//--------------------------------------------------------------------------
StatusCode TruthSeededTrackFinderTool::initialize()  {
  // Get the SCT Helper
  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------
StatusCode TruthSeededTrackFinderTool::finalize() {
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------
void TruthSeededTrackFinderTool::fillSCT_SpacePointCollection(const SpacePointCollection* clusters1, 
    SpacePointCollection* clusters2
    ) const {
  SpacePointCollection::const_iterator clusters1Next = clusters1->begin();
  SpacePointCollection::const_iterator clusters1Finish = clusters1->end();
  
  for (; clusters1Next!= clusters1Finish; ++clusters1Next){
    const Trk::SpacePoint* sp=&(**clusters1Finish);
    clusters2->push_back(const_cast<Trk::SpacePoint*>(sp));
  }


}


}
