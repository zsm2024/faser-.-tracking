/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOSIMEVENT_NEUTRINOHITCOLLECTION_H
#define NEUTRINOSIMEVENT_NEUTRINOHITCOLLECTION_H

#include "NeutrinoSimEvent/NeutrinoHit.h"
#include "HitManagement/AtlasHitsVector.h"
#include "AthenaKernel/CLASS_DEF.h"

typedef AtlasHitsVector<NeutrinoHit> NeutrinoHitCollection;
typedef AtlasHitsVector<NeutrinoHit>::iterator NeutrinoHitIterator;
typedef AtlasHitsVector<NeutrinoHit>::const_iterator NeutrinoHitConstIterator;

#ifndef __CINT__
  CLASS_DEF(NeutrinoHitCollection, 1232962600, 1 )
#endif

#endif // NEUTRINOSIMEVENT_NEUTRINOHITCOLLECTION_H
