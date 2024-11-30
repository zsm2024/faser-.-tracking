/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuPlateParameters.h"
#include "VetoNuGeometryManager.h"

#include "VetoNuDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


VetoNuPlateParameters::VetoNuPlateParameters(VetoNuDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Plate General
//
int
VetoNuPlateParameters::platePmts() const
{
  return m_rdb->plateGeneral()->getInt("NUMPMTS"); 
}

double 
VetoNuPlateParameters::plateWidth() const
{
  return m_rdb->plateGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
VetoNuPlateParameters::plateLength() const
{
  return m_rdb->plateGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
VetoNuPlateParameters::plateThickness() const
{
  return m_rdb->plateGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string VetoNuPlateParameters::plateMaterial() const
{
  return m_rdb->plateGeneral()->getString("MATERIAL");
}

