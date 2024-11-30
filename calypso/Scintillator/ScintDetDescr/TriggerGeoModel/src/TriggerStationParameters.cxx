/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerStationParameters.h"
#include "TriggerGeometryManager.h"

#include "TriggerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


TriggerStationParameters::TriggerStationParameters(TriggerDataBase* rdb)
{
  m_rdb = rdb;
}


//
// General
//
int
TriggerStationParameters::numPlates() const
{
  return m_rdb->stationGeneral()->getInt("NUMPLATES"); 
}

double
TriggerStationParameters::platePitchY() const
{
  return m_rdb->stationGeneral()->getDouble("PLATEPITCHY") * Gaudi::Units::mm;
}

double
TriggerStationParameters::platePitchZ() const
{
  return m_rdb->stationGeneral()->getDouble("PLATEPITCHZ") * Gaudi::Units::mm;
}


