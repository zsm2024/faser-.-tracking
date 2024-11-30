/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMEVENT_FASERSIHITCOLLECTION_H
#define TRACKERSIMEVENT_FASERSIHITCOLLECTION_H

#include "TrackerSimEvent/FaserSiHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AtlasHitsVector<FaserSiHit> FaserSiHitCollection;
typedef AtlasHitsVector<FaserSiHit>::iterator FaserSiHitIterator;
typedef AtlasHitsVector<FaserSiHit>::const_iterator FaserSiHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(FaserSiHitCollection, 1272740890, 1 )
#endif

#endif // TRACKERSIMEVENT_FASERSIHITCOLLECTION_H
