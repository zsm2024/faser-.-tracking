/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalGeoModel/EcalGeoModelAthenaComps.h"

EcalGeoModelAthenaComps::EcalGeoModelAthenaComps()
  : CaloDD::AthenaComps("EcalGeoModel"),
    m_idHelper(0)
{}
 
void 
EcalGeoModelAthenaComps::setIdHelper(const EcalID* idHelper)
{
  m_idHelper = idHelper;
}

const EcalID* 
EcalGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

