// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETER_VERSIONS_CALORIMETERHITCONTAINER_V1_H
#define XAODFASERCALORIMETER_VERSIONS_CALORIMETERHITCONTAINER_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// EDM include(s):
#include "AthContainers/DataVector.h"

// Local includes:
#include "xAODFaserCalorimeter/versions/CalorimeterHit_v1.h"

namespace xAOD {
  // Define the container as a simple DataVector
  typedef DataVector<CalorimeterHit_v1> CalorimeterHitContainer_v1;
}

#endif // XAODFASERCALORIMETER_VERSIONS_CALORIMETERHITCONTAINER_V1_H
