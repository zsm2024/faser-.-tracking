/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoWrapping.h"

#include "VetoMaterialManager.h"

#include "VetoGeometryManager.h"
#include "VetoGeneralParameters.h"
#include "VetoWrappingParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

VetoWrapping::VetoWrapping(const std::string & name,
                            ScintDD::VetoDetectorManager* detectorManager,
                            const VetoGeometryManager* geometryManager,
                            VetoMaterialManager* materials)
  : VetoSharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
VetoWrapping::getParameters() 
{
  const VetoWrappingParameters * parameters = m_geometryManager->wrappingParameters();
  const VetoGeneralParameters* generalParameters = m_geometryManager->generalParameters();

  m_material  = m_materials->getMaterial(parameters->wrappingMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->wrappingThickness();
  m_width     = parameters->wrappingWidth();
  m_length    = parameters->wrappingLength();
}

GeoVPhysVol * 
VetoWrapping::build()
{
  // Just a simple box.
  const GeoBox * simpleWrappingShape = new GeoBox(0.5*m_width,
                                                  0.5*m_length,
                                                  0.5*m_thickness);

  const GeoLogVol * simpleWrappingLog = 
    new GeoLogVol(getName(), simpleWrappingShape, m_material);

  GeoPhysVol * simpleWrapping = new GeoPhysVol(simpleWrappingLog);

  return simpleWrapping;
}
