/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionBaseboard.h"

#include "EmulsionGeometryManager.h"
#include "EmulsionMaterialManager.h"

#include "EmulsionFilmParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "NeutrinoReadoutGeometry/EmulsionDetectorManager.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorDesign.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorElement.h"
#include "NeutrinoReadoutGeometry/NeutrinoDD_Defs.h"
#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace NeutrinoDD;

EmulsionBaseboard::EmulsionBaseboard(const std::string & name,
                     NeutrinoDD::EmulsionDetectorManager* detectorManager,
                     const EmulsionGeometryManager* geometryManager,
                     EmulsionMaterialManager* materials)
  : EmulsionSharedComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_physVolume = build();
}


void
EmulsionBaseboard::getParameters()
{
  const EmulsionFilmParameters * parameters = m_geometryManager->filmParameters();
  m_material  = m_materials->getMaterial(parameters->baseMaterial());
  m_thickness = parameters->baseThickness();
  m_height    = parameters->baseHeight();
  m_width     = parameters->baseWidth();
}

GeoVPhysVol * 
EmulsionBaseboard::build()
{
  const GeoBox* baseboardShape = new GeoBox(0.5*m_width, 0.5*m_height, 0.5*m_thickness);

  GeoLogVol * baseboardLog = new GeoLogVol(getName(), baseboardShape, m_material);

  GeoPhysVol * baseboard = new GeoPhysVol(baseboardLog); 
  
  return baseboard;
}
