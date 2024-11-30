/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_Wrapping.h"

#include "SCT_MaterialManager.h"

#include "SCT_GeometryManager.h"
#include "SCT_GeneralParameters.h"
#include "SCT_WrappingParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

SCT_Wrapping::SCT_Wrapping(const std::string & name,
                            TrackerDD::SCT_DetectorManager* detectorManager,
                            const SCT_GeometryManager* geometryManager,
                            SCT_MaterialManager* materials)
  : SCT_SharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}

void
SCT_Wrapping::getParameters() 
{
  const SCT_WrappingParameters * parameters = m_geometryManager->wrappingParameters();
  const SCT_GeneralParameters* generalParameters = m_geometryManager->generalParameters();

  m_material  = m_materials->getMaterial(parameters->wrappingMaterial());
  m_safety    = generalParameters->safety();
  m_thickness = parameters->wrappingThickness();
  m_width     = parameters->wrappingWidth();
  m_length    = parameters->wrappingLength();
}

GeoVPhysVol * 
SCT_Wrapping::build()
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
