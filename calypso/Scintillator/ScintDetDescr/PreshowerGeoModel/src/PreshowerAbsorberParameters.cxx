/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerAbsorberParameters.h"
#include "PreshowerGeometryManager.h"

#include "PreshowerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


PreshowerAbsorberParameters::PreshowerAbsorberParameters(PreshowerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Absorber General
//
double 
PreshowerAbsorberParameters::absorberWidth() const
{
  return m_rdb->absorberGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
PreshowerAbsorberParameters::absorberLength() const
{
  return m_rdb->absorberGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
PreshowerAbsorberParameters::absorberThickness() const
{
  return m_rdb->absorberGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string PreshowerAbsorberParameters::absorberMaterial() const
{
  return m_rdb->absorberGeneral()->getString("MATERIAL");
}

