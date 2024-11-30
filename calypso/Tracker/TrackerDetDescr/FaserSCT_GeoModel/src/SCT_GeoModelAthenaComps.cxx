/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_GeoModel/SCT_GeoModelAthenaComps.h"

SCT_GeoModelAthenaComps::SCT_GeoModelAthenaComps()
  : TrackerDD::AthenaComps("SCT_GeoModel"),
    m_idHelper(0)
{}
 
void 
SCT_GeoModelAthenaComps::setIdHelper(const FaserSCT_ID* idHelper)
{
  m_idHelper = idHelper;
}

const FaserSCT_ID* 
SCT_GeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

