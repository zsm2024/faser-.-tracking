/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuRadiator.h"

#include "VetoNuMaterialManager.h"

#include "VetoNuGeometryManager.h"
#include "VetoNuGeneralParameters.h"
#include "VetoNuRadiatorParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

VetoNuRadiator::VetoNuRadiator(const std::string & name,
                            ScintDD::VetoNuDetectorManager* detectorManager,
                            const VetoNuGeometryManager* geometryManager,
                            VetoNuMaterialManager* materials)
  : VetoNuSharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
VetoNuRadiator::getParameters() 
{
  const VetoNuRadiatorParameters * parameters = m_geometryManager->radiatorParameters();
  const VetoNuGeneralParameters* generalParameters = m_geometryManager->generalParameters();

  m_material  = m_materials->getMaterial(parameters->radiatorMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->radiatorThickness();
  m_width     = parameters->radiatorWidth();
  m_length    = parameters->radiatorLength();
}

GeoVPhysVol * 
VetoNuRadiator::build()
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
