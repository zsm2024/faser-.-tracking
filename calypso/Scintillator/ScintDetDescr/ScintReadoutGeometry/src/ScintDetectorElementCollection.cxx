/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ScintReadoutGeometry/ScintDetectorElementCollection.h"

#include "ScintReadoutGeometry/ScintDetectorElement.h"
#include "Identifier/IdentifierHash.h"

ScintDD::ScintDetectorElementCollection::~ScintDetectorElementCollection() {
  for (ScintDD::ScintDetectorElement* ele: *this) delete ele;
}

const ScintDD::ScintDetectorElement*
ScintDD::ScintDetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
