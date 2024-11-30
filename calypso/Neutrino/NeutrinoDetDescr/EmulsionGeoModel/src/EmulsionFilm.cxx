/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionFilm.h"

#include "EmulsionGeometryManager.h"
#include "EmulsionMaterialManager.h"

#include "EmulsionFilmParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "NeutrinoReadoutGeometry/EmulsionDetectorManager.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorDesign.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorElement.h"
#include "NeutrinoReadoutGeometry/NeutrinoDD_Defs.h"
#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace NeutrinoDD;

EmulsionFilm::EmulsionFilm(const std::string & name,
                     NeutrinoDD::EmulsionDetectorManager* detectorManager,
                     const EmulsionGeometryManager* geometryManager,
                     EmulsionMaterialManager* materials)
  : EmulsionUniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_noElementWarning{true}
{
  getParameters();
  m_logVolume = preBuild();
}


void
EmulsionFilm::getParameters()
{
  const EmulsionFilmParameters * parameters = m_geometryManager->filmParameters();
  m_material  = m_materials->getMaterial(parameters->filmMaterial());
  m_thickness = parameters->filmThickness();
  m_height    = parameters->filmHeight();
  m_width     = parameters->filmWidth();
}

const GeoLogVol * 
EmulsionFilm::preBuild()
{
  // Simple box
  const GeoBox* filmShape = new GeoBox(0.5*m_width, 0.5*m_height, 0.5*m_thickness);

  GeoLogVol * filmLog = new GeoLogVol(getName(), filmShape, m_material);
  makeDesign();
  return filmLog;
}

void
EmulsionFilm::makeDesign()
{
    m_design = new NeutrinoDetectorDesign( m_width, m_height, m_thickness );
}



GeoVPhysVol * 
EmulsionFilm::build(EmulsionIdentifier id)
{
  GeoFullPhysVol * film = new GeoFullPhysVol(m_logVolume); 
  
  // Make detector element and add to collection
  // Only do so if we have a valid id helper.

  //id.print(); // for debugging only

  const NeutrinoCommonItems* commonItems =  m_geometryManager->commonItems();

  if (commonItems->getIdHelper()) {

    NeutrinoDetectorElement * detElement;

    detElement =  new NeutrinoDetectorElement(id.getFilmId(), 
                                              m_design, 
                                              film,  
                                              commonItems);
    // Add the detector element.
    m_detectorManager->addDetectorElement(detElement);

  } else {
    if (m_noElementWarning) {
      std::cout << "WARNING!!!!: No Emulsion id helper and so no elements being produced." << std::endl;
      m_noElementWarning = false;
    }
  }
  return film;
}
