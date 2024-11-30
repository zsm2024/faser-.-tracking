/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSGEOMETRYCONTEXT_H
#define FASERACTSGEOMETRY_ACTSGEOMETRYCONTEXT_H

#include "FaserActsGeometry/FaserActsAlignmentStore.h"

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include "Acts/Geometry/GeometryContext.hpp"

#include <memory>

struct FaserActsGeometryContext {

  bool construction{false};

  std::unique_ptr<const FaserActsAlignmentStore> ownedAlignmentStore{nullptr};

  const FaserActsAlignmentStore* alignmentStore{nullptr};

  Acts::GeometryContext
  context() const
  {
    return Acts::GeometryContext(this);
  }
};

CLASS_DEF(FaserActsGeometryContext, 123437909, 1)
CONDCONT_DEF( FaserActsGeometryContext , 27906711 );

#endif
