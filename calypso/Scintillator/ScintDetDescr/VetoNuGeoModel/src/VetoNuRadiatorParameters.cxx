/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuRadiatorParameters.h"
#include "VetoNuGeometryManager.h"

#include "VetoNuDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


VetoNuRadiatorParameters::VetoNuRadiatorParameters(VetoNuDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Radiator General
//
double 
VetoNuRadiatorParameters::radiatorWidth() const
{
  return m_rdb->radiatorGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
VetoNuRadiatorParameters::radiatorLength() const
{
  return m_rdb->radiatorGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
VetoNuRadiatorParameters::radiatorThickness() const
{
  return m_rdb->radiatorGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string VetoNuRadiatorParameters::radiatorMaterial() const
{
  return m_rdb->radiatorGeneral()->getString("MATERIAL");
}

