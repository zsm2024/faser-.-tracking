/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "FaserDetDescr/FaserDetectorID.h"

namespace TrackerDD {
  class SiDetectorElement;
}

class SCT_ID;

class IdentityHelper {

public:
  IdentityHelper(const TrackerDD::SiDetectorElement* detElem);
  
private:
  const TrackerDD::SiDetectorElement* m_elem;
  
  const FaserDetectorID* getSCTIDHelper() const;

  
};
