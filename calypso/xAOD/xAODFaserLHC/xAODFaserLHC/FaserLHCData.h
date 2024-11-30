// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERLHC_FASERLHCDATA_H
#define XAODFASERLHC_FASERLHCDATA_H

// Local include(s):
#include "xAODFaserLHC/versions/FaserLHCData_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef FaserLHCData_v1 FaserLHCData;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FaserLHCData, 255278152, 1 )

#endif // XAODFASERLHC_FASERLHCDATA_H
