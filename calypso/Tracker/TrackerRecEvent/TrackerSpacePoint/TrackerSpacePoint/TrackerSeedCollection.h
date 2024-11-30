#ifndef TRACKERSEEDCOLLECTION_H
#define TRACKERSEEDCOLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/DataObject.h"
#include "TrackerSpacePoint/TrackerSeed.h"

namespace Tracker {
  typedef DataVector<TrackerSeed> TrackerSeedCollection;
}

CLASS_DEF(Tracker::TrackerSeedCollection, 1139531114, 1)

#endif // TRACKERSEEDCOLLECTION_H

