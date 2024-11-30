/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuGeoModel/VetoNuGeoModelAthenaComps.h"

VetoNuGeoModelAthenaComps::VetoNuGeoModelAthenaComps()
  : ScintDD::AthenaComps("VetoNuGeoModel"),
    m_idHelper(0)
{}
 
void 
VetoNuGeoModelAthenaComps::setIdHelper(const VetoNuID* idHelper)
{
  m_idHelper = idHelper;
}

const VetoNuID* 
VetoNuGeoModelAthenaComps::getIdHelper() const
{
  return m_idHelper;
}

