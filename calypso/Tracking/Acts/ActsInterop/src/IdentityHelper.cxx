/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsInterop/IdentityHelper.h"

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "FaserDetDescr/FaserDetectorID.h"


IdentityHelper::IdentityHelper(const TrackerDD::SiDetectorElement *elem)
  : m_elem(elem)
{}
  

const FaserDetectorID* 
IdentityHelper::getSCTIDHelper() const
{
  return dynamic_cast<const FaserDetectorID*>(m_elem->getIdHelper());
}

