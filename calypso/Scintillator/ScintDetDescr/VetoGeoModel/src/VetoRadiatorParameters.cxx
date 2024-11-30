/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoRadiatorParameters.h"
#include "VetoGeometryManager.h"

#include "VetoDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


VetoRadiatorParameters::VetoRadiatorParameters(VetoDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Radiator General
//
double 
VetoRadiatorParameters::radiatorWidth() const
{
  return m_rdb->radiatorGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
VetoRadiatorParameters::radiatorLength() const
{
  return m_rdb->radiatorGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
VetoRadiatorParameters::radiatorThickness() const
{
  return m_rdb->radiatorGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string VetoRadiatorParameters::radiatorMaterial() const
{
  return m_rdb->radiatorGeneral()->getString("MATERIAL");
}

