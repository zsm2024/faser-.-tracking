/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerGeoModel/TriggerGeoModelAthenaComps.h"

TriggerGeoModelAthenaComps::TriggerGeoModelAthenaComps()
  : ScintDD::AthenaComps("TriggerGeoModel"),
    m_idHelper(0)
{}
 
void 
TriggerGeoModelAthenaComps::setIdHelper(const TriggerID* idHelper)
{
  m_idHelper = idHelper;
}

const TriggerID* 
TriggerGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

