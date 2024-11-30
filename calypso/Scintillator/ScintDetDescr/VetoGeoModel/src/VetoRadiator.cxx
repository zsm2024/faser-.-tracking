/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoRadiator.h"

#include "VetoMaterialManager.h"

#include "VetoGeometryManager.h"
#include "VetoGeneralParameters.h"
#include "VetoRadiatorParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

VetoRadiator::VetoRadiator(const std::string & name,
                            ScintDD::VetoDetectorManager* detectorManager,
                            const VetoGeometryManager* geometryManager,
                            VetoMaterialManager* materials)
  : VetoSharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
VetoRadiator::getParameters() 
{
  const VetoRadiatorParameters * parameters = m_geometryManager->radiatorParameters();
  const VetoGeneralParameters* generalParameters = m_geometryManager->generalParameters();

  m_material  = m_materials->getMaterial(parameters->radiatorMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->radiatorThickness();
  m_width     = parameters->radiatorWidth();
  m_length    = parameters->radiatorLength();
}

GeoVPhysVol * 
VetoRadiator::build()
{
  // Just a simple box.
  const GeoBox * simpleRadiatorShape = new GeoBox(0.5*m_width,
                                                  0.5*m_length,
                                                  0.5*m_thickness);

  const GeoLogVol * simpleRadiatorLog = 
    new GeoLogVol(getName(), simpleRadiatorShape, m_material);

  GeoPhysVol * simpleRadiator = new GeoPhysVol(simpleRadiatorLog);

  return simpleRadiator;
}
