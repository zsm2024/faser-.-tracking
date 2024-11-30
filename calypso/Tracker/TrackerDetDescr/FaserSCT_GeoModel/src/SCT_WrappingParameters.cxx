/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_WrappingParameters.h"
#include "SCT_GeometryManager.h"

#include "SCT_DataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


SCT_WrappingParameters::SCT_WrappingParameters(SCT_DataBase* rdb)
{
  m_rdb = rdb;
}

//
// Wrapping General
//
double 
SCT_WrappingParameters::wrappingWidth() const
{
  return m_rdb->wrappingGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
SCT_WrappingParameters::wrappingLength() const
{
  return m_rdb->wrappingGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
SCT_WrappingParameters::wrappingThickness() const
{
  return m_rdb->wrappingGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string SCT_WrappingParameters::wrappingMaterial() const
{
  return m_rdb->wrappingGeneral()->getString("MATERIAL");
}

