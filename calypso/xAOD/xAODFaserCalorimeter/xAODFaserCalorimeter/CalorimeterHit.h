// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETER_CALORIMETERHIT_H
#define XAODFASERCALORIMETER_CALORIMETERHIT_H

// Local include(s):
#include "xAODFaserCalorimeter/versions/CalorimeterHit_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef CalorimeterHit_v1 CalorimeterHit;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CalorimeterHit, 195445226, 1 )

#endif // XAODFASERCALORIMETER_CALORIMETERHIT_H
