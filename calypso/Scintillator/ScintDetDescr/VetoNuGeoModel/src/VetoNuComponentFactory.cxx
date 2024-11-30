/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using ScintDD::VetoNuDetectorManager;

const double VetoNuComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

VetoNuComponentFactory::VetoNuComponentFactory(const std::string & name,
                                           ScintDD::VetoNuDetectorManager* detectorManager,
                                           const VetoNuGeometryManager* geometryManager,
                                           VetoNuMaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

VetoNuComponentFactory::~VetoNuComponentFactory() 
{}

std::string 
VetoNuComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
VetoNuComponentFactory::epsilon() const
{
  return s_epsilon;
}
