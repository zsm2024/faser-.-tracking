/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerPlateParameters.h"
#include "TriggerGeometryManager.h"

#include "TriggerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


TriggerPlateParameters::TriggerPlateParameters(TriggerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Plate General
//
int
TriggerPlateParameters::platePmts() const
{
  return m_rdb->plateGeneral()->getInt("NUMPMTS"); 
}

double 
TriggerPlateParameters::plateWidth() const
{
  return m_rdb->plateGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
TriggerPlateParameters::plateLength() const
{
  return m_rdb->plateGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
TriggerPlateParameters::plateThickness() const
{
  return m_rdb->plateGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string TriggerPlateParameters::plateMaterial() const
{
  return m_rdb->plateGeneral()->getString("MATERIAL");
}

