// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef XAODFASERCALORIMETER_CALORIMETERHITAUXCONTAINER_H
#define XAODFASERCALORIMETER_CALORIMETERHITAUXCONTAINER_H

// Local include(s):
#include "xAODFaserCalorimeter/versions/CalorimeterHitAuxContainer_v1.h"

namespace xAOD {
  /// Declare the latest version of the class
  typedef CalorimeterHitAuxContainer_v1 CalorimeterHitAuxContainer;
}

// Set up a CLID for the container:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::CalorimeterHitAuxContainer, 1074912337, 1 )

#endif // XAODFASERCALORIMETER_CALORIMETERHITAUXCONTAINER_H
