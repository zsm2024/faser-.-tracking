/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsGeometry/FaserActsAlignmentStore.h"

#include "FaserActsGeometry/FaserActsDetectorElement.h"

#include "Acts/Definitions/Common.hpp"

FaserActsAlignmentStore::FaserActsAlignmentStore(const GeoAlignmentStore &gas){

  m_deltas = gas.getDeltas();
  m_absPositions = gas.getAbsPositions();
  m_defAbsPositions = gas.getDefAbsPositions();

}

void FaserActsAlignmentStore::setTransform(const FaserActsDetectorElement *ade,
                                      const Acts::Transform3 &xf) {
  if (!m_transforms.setTransform(ade, xf)) {
    throw ExcAlignmentStore(
        "Attempted to overwrite Delta in the Alignment Store");
  }
}

const Acts::Transform3 *
FaserActsAlignmentStore::getTransform(const FaserActsDetectorElement *ade) const {
  return m_transforms.getTransform(ade);
}

void FaserActsAlignmentStore::append(const GeoAlignmentStore& gas) {
  for(const auto& it : gas.getDeltas().container()) {
    setDelta(it.first, it.second);
  }

  for(const auto& it : gas.getAbsPositions().container()) {
    setAbsPosition(it.first, it.second);
  }

  for(const auto& it : gas.getDefAbsPositions().container()) {
    setDefAbsPosition(it.first, it.second);
  }
}
