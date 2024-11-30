// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERTRIGGER_FASERTRIGGERDATAAUX_H
#define XAODFASERTRIGGER_FASERTRIGGERDATAAUX_H

// Local include(s):
#include "xAODFaserTrigger/versions/FaserTriggerDataAux_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef FaserTriggerDataAux_v1 FaserTriggerDataAux;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::FaserTriggerDataAux, 155975504, 1 )

#endif // XAODFASERTRIGGER_FASERTRIGGERDATAAUX_H
