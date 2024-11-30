
#ifndef SPACEPOINTFORSEED_SPACEPOINTCOLLECTION_H
#define SPACEPOINTFORSEED_SPACEPOINTCOLLECTION_H

// Base classes
#include "AthContainers/DataVector.h"
#include "GaudiKernel/DataObject.h"
//class SpacePoint;
#include "TrackerSpacePoint/SpacePointForSeed.h"



typedef DataVector<SpacePointForSeed> SpacePointForSeedCollection;

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(SpacePointForSeedCollection,1123314789,1)

#endif // TRKSPACEPOINT_SPACEPOINTCOLLECTION_H
