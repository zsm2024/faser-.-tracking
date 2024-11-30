/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "Identifier/IdentifierHash.h"

TrackerDD::SiDetectorElementCollection::~SiDetectorElementCollection() {
  for (TrackerDD::SiDetectorElement* ele: *this) delete ele;
}

const TrackerDD::SiDetectorElement*
TrackerDD::SiDetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
