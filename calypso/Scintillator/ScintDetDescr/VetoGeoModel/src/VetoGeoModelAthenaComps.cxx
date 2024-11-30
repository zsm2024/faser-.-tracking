/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoGeoModel/VetoGeoModelAthenaComps.h"

VetoGeoModelAthenaComps::VetoGeoModelAthenaComps()
  : ScintDD::AthenaComps("VetoGeoModel"),
    m_idHelper(0)
{}
 
void 
VetoGeoModelAthenaComps::setIdHelper(const VetoID* idHelper)
{
  m_idHelper = idHelper;
}

const VetoID* 
VetoGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

