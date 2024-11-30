// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERLHC_FASERLHCDATAAUX_H
#define XAODFASERLHC_FASERLHCDATAAUX_H

// Local include(s):
#include "xAODFaserLHC/versions/FaserLHCDataAux_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef FaserLHCDataAux_v1 FaserLHCDataAux;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FaserLHCDataAux, 151779941, 1 )

#endif // XAODFASERLHC_FASERLHCDATAAUX_H
