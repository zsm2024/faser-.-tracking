/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionPlatesParameters.h"
#include "EmulsionGeometryManager.h"

#include "EmulsionDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


EmulsionPlatesParameters::EmulsionPlatesParameters(EmulsionDataBase* rdb)
{
  m_rdb = rdb;
}

double 
EmulsionPlatesParameters::platesWidth() const
{
  return m_rdb->emulsionPlates()->getDouble("DX");
}

double 
EmulsionPlatesParameters::platesHeight() const
{
  return m_rdb->emulsionPlates()->getDouble("DY");
}

double 
EmulsionPlatesParameters::platesThickness() const
{
  return m_rdb->emulsionPlates()->getDouble("DZ");
}

double 
EmulsionPlatesParameters::platesX() const
{
  return m_rdb->emulsionPlates()->getDouble("POSX");
}

double 
EmulsionPlatesParameters::platesY() const
{
  return m_rdb->emulsionPlates()->getDouble("POSY");
}

double 
EmulsionPlatesParameters::platesZ() const
{
  return m_rdb->emulsionPlates()->getDouble("POSZ");
}

std::string 
EmulsionPlatesParameters::platesMaterial() const
{
  return m_rdb->emulsionPlates()->getString("MATERIAL");
}