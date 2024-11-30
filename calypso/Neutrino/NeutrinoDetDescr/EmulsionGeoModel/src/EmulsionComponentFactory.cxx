/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using NeutrinoDD::EmulsionDetectorManager;

const double EmulsionComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

EmulsionComponentFactory::EmulsionComponentFactory(const std::string & name,
                                           NeutrinoDD::EmulsionDetectorManager* detectorManager,
                                           const EmulsionGeometryManager* geometryManager,
                                           EmulsionMaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

EmulsionComponentFactory::~EmulsionComponentFactory() 
{}

std::string 
EmulsionComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
EmulsionComponentFactory::epsilon() const
{
  return s_epsilon;
}
