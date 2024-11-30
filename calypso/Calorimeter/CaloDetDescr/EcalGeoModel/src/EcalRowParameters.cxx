/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalRowParameters.h"
#include "EcalGeometryManager.h"

#include "EcalDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


EcalRowParameters::EcalRowParameters(EcalDataBase* rdb)
{
  m_rdb = rdb;
}

//
// General
//
int
EcalRowParameters::numModules() const
{
  return m_rdb->rowGeneral()->getInt("NUMMODULES"); 
}

double
EcalRowParameters::xGap() const
{
  return m_rdb->rowGeneral()->getDouble("XGAP");
}

double
EcalRowParameters::yawAngle() const
{
  return m_rdb->rowGeneral()->getDouble("YAWANGLE");
}

