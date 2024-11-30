/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_IACTSPROPSTEPROOTWRITERSVC_H
#define FASERACTSGEOMETRY_IACTSPROPSTEPROOTWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {
  namespace detail {
    class Step;
  }
}


class IFaserActsPropStepRootWriterSvc : virtual public IInterface {
public:
    
  DeclareInterfaceID(IFaserActsPropStepRootWriterSvc, 1, 0);

  IFaserActsPropStepRootWriterSvc() {;}
    
  void
  virtual
  write(const std::vector<Acts::detail::Step>& steps) = 0;

};


#endif 
