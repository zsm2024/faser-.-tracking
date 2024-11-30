/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author Elmar.Ritsch -at- cern.ch
 * @author Robert.Langenberg -at- cern.ch
 * @date March 2017
 * @brief Thread-local storage object used by MagField::FaserFieldSvc
 */

#ifndef MAGFIELDSERVICES_FASERFIELDSVCTLS_H
#define MAGFIELDSERVICES_FASERFIELDSVCTLS_H 1

// MagField includes
#include "MagFieldElements/BFieldZone.h"

namespace MagField {

/** @class FaserFieldSvcTLS
 *
 *  @brief Thread-local storage object used by MagField::FaserFieldSvc
 *
 */
struct FaserFieldSvcTLS {

  /// Constructor
  FaserFieldSvcTLS() : isInitialized(false), /*cond(nullptr),*/ cache() /*, cacheZR()*/ { ; }

  /// Is the current FaserFieldSvcTLS object properly initialized
  bool isInitialized;

  /// Pointer to the conductors in the current field zone (to compute Biot-Savart component)
//   const std::vector<BFieldCond> *cond;

  /// Full 3d field
  BFieldCache cache;
  /// Fast 2d field
//   BFieldCacheZR cacheZR;
};

}  // namespace MagField

#endif  // MAGFIELDSERVICES_FASERFIELDSVCTLS_H
