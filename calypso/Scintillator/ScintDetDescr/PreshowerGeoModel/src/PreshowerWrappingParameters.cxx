/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerWrappingParameters.h"
#include "PreshowerGeometryManager.h"

#include "PreshowerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


PreshowerWrappingParameters::PreshowerWrappingParameters(PreshowerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Wrapping General
//
double 
PreshowerWrappingParameters::wrappingWidth() const
{
  return m_rdb->wrappingGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
PreshowerWrappingParameters::wrappingLength() const
{
  return m_rdb->wrappingGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
PreshowerWrappingParameters::wrappingThickness() const
{
  return m_rdb->wrappingGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string PreshowerWrappingParameters::wrappingMaterial() const
{
  return m_rdb->wrappingGeneral()->getString("MATERIAL");
}

