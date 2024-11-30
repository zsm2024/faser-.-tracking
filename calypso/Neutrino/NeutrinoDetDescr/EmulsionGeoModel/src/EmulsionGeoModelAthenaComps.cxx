/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionGeoModel/EmulsionGeoModelAthenaComps.h"

EmulsionGeoModelAthenaComps::EmulsionGeoModelAthenaComps()
  : NeutrinoDD::AthenaComps("EmulsionGeoModel"),
    m_idHelper(0)
{}
 
void 
EmulsionGeoModelAthenaComps::setIdHelper(const EmulsionID* idHelper)
{
  m_idHelper = idHelper;
}

const EmulsionID* 
EmulsionGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

