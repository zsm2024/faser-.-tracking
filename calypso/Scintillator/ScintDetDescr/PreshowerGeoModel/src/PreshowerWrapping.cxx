/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerWrapping.h"

#include "PreshowerMaterialManager.h"

#include "PreshowerGeometryManager.h"
#include "PreshowerGeneralParameters.h"
#include "PreshowerWrappingParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

PreshowerWrapping::PreshowerWrapping(const std::string & name,
                            ScintDD::PreshowerDetectorManager* detectorManager,
                            const PreshowerGeometryManager* geometryManager,
                            PreshowerMaterialManager* materials)
  : PreshowerSharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
PreshowerWrapping::getParameters() 
{
  const PreshowerWrappingParameters * parameters = m_geometryManager->wrappingParameters();
  const PreshowerGeneralParameters* generalParameters = m_geometryManager->generalParameters();

  m_material  = m_materials->getMaterial(parameters->wrappingMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->wrappingThickness();
  m_width     = parameters->wrappingWidth();
  m_length    = parameters->wrappingLength();
}

GeoVPhysVol * 
PreshowerWrapping::build()
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
