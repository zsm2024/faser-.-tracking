/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerRadiatorParameters.h"
#include "PreshowerGeometryManager.h"

#include "PreshowerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


PreshowerRadiatorParameters::PreshowerRadiatorParameters(PreshowerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Radiator General
//
double 
PreshowerRadiatorParameters::radiatorWidth() const
{
  return m_rdb->radiatorGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
PreshowerRadiatorParameters::radiatorLength() const
{
  return m_rdb->radiatorGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
PreshowerRadiatorParameters::radiatorThickness() const
{
  return m_rdb->radiatorGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string PreshowerRadiatorParameters::radiatorMaterial() const
{
  return m_rdb->radiatorGeneral()->getString("MATERIAL");
}

