/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using ScintDD::VetoDetectorManager;

const double VetoComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

VetoComponentFactory::VetoComponentFactory(const std::string & name,
                                           ScintDD::VetoDetectorManager* detectorManager,
                                           const VetoGeometryManager* geometryManager,
                                           VetoMaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

VetoComponentFactory::~VetoComponentFactory() 
{}

std::string 
VetoComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
VetoComponentFactory::epsilon() const
{
  return s_epsilon;
}
