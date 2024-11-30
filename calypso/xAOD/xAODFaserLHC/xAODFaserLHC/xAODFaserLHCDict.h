/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERLHC_XAODFASERLHCDICT_H
#define XAODFASERLHC_XAODFASERLHCDICT_H

// Local include(s):
#include "xAODFaserLHC/FaserLHCData.h"
#include "xAODFaserLHC/FaserLHCDataAux.h"
#include "xAODFaserLHC/versions/FaserLHCData_v1.h"
#include "xAODFaserLHC/versions/FaserLHCDataAux_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

namespace {
  struct GCCXML_DUMMY_INSTANTIATION_XAODFASERLHC {
    // Local type(s).
    XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, FaserLHCData_v1 );

  };
}


#endif // XAODFASERLHC_XAODFASERLHCDICT_H

