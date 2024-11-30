/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSALIGNMENTSTORE_H
#define FASERACTSGEOMETRY_ACTSALIGNMENTSTORE_H

#include "GeoModelFaserUtilities/GeoAlignmentStore.h"
#include "GeoModelFaserUtilities/TransformMap.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"

#include "Acts/Definitions/Algebra.hpp"

#include <stdexcept>

class FaserActsDetectorElement;

class FaserActsAlignmentStore : public GeoAlignmentStore
{
  public:
    FaserActsAlignmentStore() {}
    FaserActsAlignmentStore(const GeoAlignmentStore& gas);

    void setTransform(const FaserActsDetectorElement* key, const Acts::Transform3&);
    const Acts::Transform3* getTransform(const FaserActsDetectorElement* key) const;

    void append(const GeoAlignmentStore& gas);

  private:
    TransformMap<FaserActsDetectorElement, Acts::Transform3> m_transforms;
};

CLASS_DEF(FaserActsAlignmentStore, 58650257, 1)
CONDCONT_DEF( FaserActsAlignmentStore , 124731561 );

#endif
