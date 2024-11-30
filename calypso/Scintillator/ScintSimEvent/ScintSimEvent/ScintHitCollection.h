/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTSIMEVENT_SCINTHITCOLLECTION_H
#define SCINTSIMEVENT_SCINTHITCOLLECTION_H

#include "ScintSimEvent/ScintHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AtlasHitsVector<ScintHit> ScintHitCollection;
typedef AtlasHitsVector<ScintHit>::iterator ScintHitIterator;
typedef AtlasHitsVector<ScintHit>::const_iterator ScintHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(ScintHitCollection, 1209245270, 1 )
#endif

#endif // SCINTSIMEVENT_SCINTHITCOLLECTION_H
