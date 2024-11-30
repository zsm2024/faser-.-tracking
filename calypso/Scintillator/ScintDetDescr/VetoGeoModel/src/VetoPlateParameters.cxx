/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoPlateParameters.h"
#include "VetoGeometryManager.h"

#include "VetoDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


VetoPlateParameters::VetoPlateParameters(VetoDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Plate General
//
int
VetoPlateParameters::platePmts() const
{
  return m_rdb->plateGeneral()->getInt("NUMPMTS"); 
}

double 
VetoPlateParameters::plateWidth() const
{
  return m_rdb->plateGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
VetoPlateParameters::plateLength() const
{
  return m_rdb->plateGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
VetoPlateParameters::plateThickness() const
{
  return m_rdb->plateGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string VetoPlateParameters::plateMaterial() const
{
  return m_rdb->plateGeneral()->getString("MATERIAL");
}

