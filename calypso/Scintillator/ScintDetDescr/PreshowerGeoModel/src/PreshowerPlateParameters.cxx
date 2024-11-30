/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerPlateParameters.h"
#include "PreshowerGeometryManager.h"

#include "PreshowerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


PreshowerPlateParameters::PreshowerPlateParameters(PreshowerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Plate General
//
int
PreshowerPlateParameters::platePmts() const
{
  return m_rdb->plateGeneral()->getInt("NUMPMTS"); 
}

double 
PreshowerPlateParameters::plateWidth() const
{
  return m_rdb->plateGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
PreshowerPlateParameters::plateLength() const
{
  return m_rdb->plateGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
PreshowerPlateParameters::plateThickness() const
{
  return m_rdb->plateGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string PreshowerPlateParameters::plateMaterial() const
{
  return m_rdb->plateGeneral()->getString("MATERIAL");
}

