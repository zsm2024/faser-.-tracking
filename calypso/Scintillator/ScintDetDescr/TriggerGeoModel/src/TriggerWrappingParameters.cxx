/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerWrappingParameters.h"
#include "TriggerGeometryManager.h"

#include "TriggerDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


TriggerWrappingParameters::TriggerWrappingParameters(TriggerDataBase* rdb)
{
  m_rdb = rdb;
}

//
// Wrapping General
//
double 
TriggerWrappingParameters::wrappingWidth() const
{
  return m_rdb->wrappingGeneral()->getDouble("WIDTH") * Gaudi::Units::mm; 
}

double 
TriggerWrappingParameters::wrappingLength() const
{
  return m_rdb->wrappingGeneral()->getDouble("LENGTH") * Gaudi::Units::mm; 
}

double 
TriggerWrappingParameters::wrappingThickness() const
{
  return m_rdb->wrappingGeneral()->getDouble("THICKNESS") * Gaudi::Units::mm; 
}

std::string TriggerWrappingParameters::wrappingMaterial() const
{
  return m_rdb->wrappingGeneral()->getString("MATERIAL");
}

