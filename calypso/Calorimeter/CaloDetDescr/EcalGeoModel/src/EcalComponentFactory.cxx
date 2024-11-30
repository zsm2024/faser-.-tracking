/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using CaloDD::EcalDetectorManager;

const double EcalComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

EcalComponentFactory::EcalComponentFactory(const std::string & name,
                                           CaloDD::EcalDetectorManager* detectorManager,
                                           const EcalGeometryManager* geometryManager,
                                           EcalMaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

EcalComponentFactory::~EcalComponentFactory() 
{}

std::string 
EcalComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
EcalComponentFactory::epsilon() const
{
  return s_epsilon;
}
