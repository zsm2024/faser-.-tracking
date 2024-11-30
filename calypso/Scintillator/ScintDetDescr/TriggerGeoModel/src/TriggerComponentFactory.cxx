/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerComponentFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <string>

using ScintDD::TriggerDetectorManager;

const double TriggerComponentFactory::s_epsilon = 1.0e-6 * Gaudi::Units::mm;

TriggerComponentFactory::TriggerComponentFactory(const std::string & name,
                                           ScintDD::TriggerDetectorManager* detectorManager,
                                           const TriggerGeometryManager* geometryManager,
                                           TriggerMaterialManager* materials)
  : m_detectorManager(detectorManager), 
    m_geometryManager(geometryManager),
    m_materials(materials),
    m_name(name)
{}

TriggerComponentFactory::~TriggerComponentFactory() 
{}

std::string 
TriggerComponentFactory::intToString(int i) const
{
  std::ostringstream str;
  str << i;
  return str.str();
}

double
TriggerComponentFactory::epsilon() const
{
  return s_epsilon;
}
