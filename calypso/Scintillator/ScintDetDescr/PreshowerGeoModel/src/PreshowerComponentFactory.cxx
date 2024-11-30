/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using ScintDD::PreshowerDetectorManager;

const double PreshowerComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

PreshowerComponentFactory::PreshowerComponentFactory(const std::string & name,
                                           ScintDD::PreshowerDetectorManager* detectorManager,
                                           const PreshowerGeometryManager* geometryManager,
                                           PreshowerMaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

PreshowerComponentFactory::~PreshowerComponentFactory() 
{}

std::string 
PreshowerComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
PreshowerComponentFactory::epsilon() const
{
  return s_epsilon;
}
