/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoWrappingParameters.h"
#include "VetoGeometryManager.h"

#include "VetoDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


VetoWrappingParameters::VetoWrappingParameters(VetoDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Wrapping General
//
double 
VetoWrappingParameters::wrappingWidth() const
{
  return m_rdb->wrappingGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
VetoWrappingParameters::wrappingLength() const
{
  return m_rdb->wrappingGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
VetoWrappingParameters::wrappingThickness() const
{
  return m_rdb->wrappingGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string VetoWrappingParameters::wrappingMaterial() const
{
  return m_rdb->wrappingGeneral()->getString("MATERIAL");
}

