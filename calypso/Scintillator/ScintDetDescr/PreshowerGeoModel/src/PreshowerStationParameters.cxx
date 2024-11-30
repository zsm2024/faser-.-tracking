/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerStationParameters.h"
#include "PreshowerGeometryManager.h"

#include "PreshowerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


PreshowerStationParameters::PreshowerStationParameters(PreshowerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// General
//
int
PreshowerStationParameters::numPlates() const
{
  return m_rdb->stationGeneral()->getInt("NUMPLATES"); 
}

double
PreshowerStationParameters::platePitch() const
{
  return m_rdb->stationGeneral()->getDouble("PLATEPITCH");
}

