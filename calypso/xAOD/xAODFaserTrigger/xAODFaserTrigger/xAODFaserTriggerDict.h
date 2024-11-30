/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERTRIGGER_XAODFASERTRIGGERDICT_H
#define XAODFASERTRIGGER_XAODFASERTRIGGERDICT_H

// Local include(s):
#include "xAODFaserTrigger/FaserTriggerData.h"
#include "xAODFaserTrigger/FaserTriggerDataAux.h"
#include "xAODFaserTrigger/versions/FaserTriggerData_v1.h"
#include "xAODFaserTrigger/versions/FaserTriggerDataAux_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODFASERTRIGGER {
    // Local type(s).
    //XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, FaserTriggerDataContainer_v1 );
    XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, FaserTriggerData_v1 );

  };
}


#endif // XAODFASERTRIGGER_XAODFASERTRIGGERDICT_H

