/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerGeoModel/PreshowerGeoModelAthenaComps.h"

PreshowerGeoModelAthenaComps::PreshowerGeoModelAthenaComps()
  : ScintDD::AthenaComps("PreshowerGeoModel"),
    m_idHelper(0)
{}
 
void 
PreshowerGeoModelAthenaComps::setIdHelper(const PreshowerID* idHelper)
{
  m_idHelper = idHelper;
}

const PreshowerID* 
PreshowerGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

