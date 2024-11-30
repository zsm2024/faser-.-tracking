/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerWrapping.h"

#include "TriggerMaterialManager.h"

#include "TriggerGeometryManager.h"
#include "TriggerGeneralParameters.h"
#include "TriggerWrappingParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

TriggerWrapping::TriggerWrapping(const std::string & name,
                            ScintDD::TriggerDetectorManager* detectorManager,
                            const TriggerGeometryManager* geometryManager,
                            TriggerMaterialManager* materials)
  : TriggerSharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
TriggerWrapping::getParameters() 
{
  const TriggerWrappingParameters * parameters = m_geometryManager->wrappingParameters();
  const TriggerGeneralParameters* generalParameters = m_geometryManager->generalParameters();

  m_material  = m_materials->getMaterial(parameters->wrappingMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->wrappingThickness();
  m_width     = parameters->wrappingWidth();
  m_length    = parameters->wrappingLength();
}

GeoVPhysVol * 
TriggerWrapping::build()
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
