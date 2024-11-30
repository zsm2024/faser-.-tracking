/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_IACTSEXCELLWRITERSVC_H
#define FASERACTSGEOMETRY_IACTSEXCELLWRITERSVC_H

#include "GaudiKernel/IInterface.h"
#include "Acts/EventData/TrackParameters.hpp"

namespace Acts {

template <class>
class ExtrapolationCell;

}


class IFaserActsExCellWriterSvc : virtual public IInterface {
public:
    
  DeclareInterfaceID(IFaserActsExCellWriterSvc, 1, 0);

  IFaserActsExCellWriterSvc() {;}
    
  void
  virtual
  store(std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>& ecells) = 0;

};


#endif 
