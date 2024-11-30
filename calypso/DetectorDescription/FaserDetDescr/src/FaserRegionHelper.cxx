/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserRegionName.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "FaserDetDescr/FaserRegionHelper.h"

namespace FaserDetDescr {

  const char * FaserRegionHelper::getName( int region ) {
    return getName( FaserDetDescr::FaserRegion( region ) );
  }

  const char * FaserRegionHelper::getName( const FaserDetDescr::FaserRegion region ) {

    if      ( region == FaserDetDescr::fFaserScintillator   ) return "FaserScint";
    else if ( region == FaserDetDescr::fFaserTracker        ) return "FaserTracker";
    else if ( region == FaserDetDescr::fFaserDipole         ) return "FaserDipole";
    else if ( region == FaserDetDescr::fFaserCalorimeter    ) return "FaserCalo";
    else if ( region == FaserDetDescr::fFaserNeutrino       ) return "FaserNeutrino";
    else if ( region == FaserDetDescr::fFaserCavern         ) return "FaserCavern";
    else if ( region == FaserDetDescr::fFaserTrench         ) return "FaserTrench";
    else                                                      return "UndefinedFaserRegion";

  }

} // end of namespace

