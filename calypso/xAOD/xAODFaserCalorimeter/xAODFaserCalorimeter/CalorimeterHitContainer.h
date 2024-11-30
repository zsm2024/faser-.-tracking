// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETER_CALORIMETERHITCONTAINER_H
#define XAODFASERCALORIMETER_CALORIMETERHITCONTAINER_H

// Local include(s):
#include "xAODFaserCalorimeter/versions/CalorimeterHitContainer_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef CalorimeterHitContainer_v1 CalorimeterHitContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CalorimeterHitContainer, 1147607550, 1 )

#endif // XAODFASERCALORIMETER_CALORIMETERHITCONTAINER_H
