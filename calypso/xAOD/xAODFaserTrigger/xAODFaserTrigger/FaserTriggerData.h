// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERTRIGGER_FASERTRIGGERDATA_H
#define XAODFASERTRIGGER_FASERTRIGGERDATA_H

// Local include(s):
#include "xAODFaserTrigger/versions/FaserTriggerData_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef FaserTriggerData_v1 FaserTriggerData;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FaserTriggerData, 266142943, 1 )

#endif // XAODFASERTRIGGER_FASERTRIGGERDATA_H
