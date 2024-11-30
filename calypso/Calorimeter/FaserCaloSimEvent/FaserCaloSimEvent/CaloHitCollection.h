/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENT_CALOHITCOLLECTION_H
#define CALOSIMEVENT_CALOHITCOLLECTION_H

#include "FaserCaloSimEvent/CaloHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AtlasHitsVector<CaloHit> CaloHitCollection;
typedef AtlasHitsVector<CaloHit>::iterator CaloHitIterator;
typedef AtlasHitsVector<CaloHit>::const_iterator CaloHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(CaloHitCollection, 1255329155, 1 )
#endif

#endif // CALOSIMEVENT_CALOHITCOLLECTION_H
