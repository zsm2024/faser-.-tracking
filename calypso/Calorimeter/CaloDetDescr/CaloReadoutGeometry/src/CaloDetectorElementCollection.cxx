/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloReadoutGeometry/CaloDetectorElementCollection.h"

#include "CaloReadoutGeometry/CaloDetectorElement.h"
#include "Identifier/IdentifierHash.h"

CaloDD::CaloDetectorElementCollection::~CaloDetectorElementCollection() {
  for (CaloDD::CaloDetectorElement* ele: *this) delete ele;
}

const CaloDD::CaloDetectorElement*
CaloDD::CaloDetectorElementCollection::getDetectorElement(const IdentifierHash& hash) const {
  const unsigned int value{hash.value()};
  if (this->size()<=value) return nullptr;
  return this->at(value);  
}
