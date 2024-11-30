/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETER_XAODFASERCALORIMETERDICT_H
#define XAODFASERCALORIMETER_XAODFASERCALORIMETERDICT_H

// Local includes
#include "xAODFaserCalorimeter/CalorimeterHit.h"
#include "xAODFaserCalorimeter/CalorimeterHitContainer.h"
#include "xAODFaserCalorimeter/CalorimeterHitAuxContainer.h"

#include "xAODFaserCalorimeter/versions/CalorimeterHit_v1.h"
#include "xAODFaserCalorimeter/versions/CalorimeterHitContainer_v1.h"
#include "xAODFaserCalorimeter/versions/CalorimeterHitAuxContainer_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODFASERCALORIMETER {
    XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, CalorimeterHitContainer_v1 );
  };
}

#endif // XAODFASERCALORIMETER_XAODFASERCALORIMETERDICT_H
