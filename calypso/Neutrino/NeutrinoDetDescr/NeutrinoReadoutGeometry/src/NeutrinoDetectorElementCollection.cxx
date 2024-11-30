/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutrinoReadoutGeometry/NeutrinoDetectorElementCollection.h"

#include "NeutrinoReadoutGeometry/NeutrinoDetectorElement.h"
#include "Identifier/IdentifierHash.h"

NeutrinoDD::NeutrinoDetectorElementCollection::~NeutrinoDetectorElementCollection() {
  for (NeutrinoDD::NeutrinoDetectorElement* ele: *this) delete ele;
}

const NeutrinoDD::NeutrinoDetectorElement*
NeutrinoDD::NeutrinoDetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
