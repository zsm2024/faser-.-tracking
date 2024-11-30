/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionBase.h"

#include "EmulsionGeometryManager.h"
#include "EmulsionMaterialManager.h"

#include "EmulsionFilmParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

#include "NeutrinoReadoutGeometry/EmulsionDetectorManager.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorDesign.h"
#include "NeutrinoReadoutGeometry/NeutrinoDetectorElement.h"
#include "NeutrinoReadoutGeometry/NeutrinoDD_Defs.h"
#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace NeutrinoDD;

EmulsionBase::EmulsionBase(const std::string & name,
                     NeutrinoDD::EmulsionDetectorManager* detectorManager,
                     const EmulsionGeometryManager* geometryManager,
                     EmulsionMaterialManager* materials)
  : EmulsionUniqueComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_logVolume = preBuild();
}


void
EmulsionBase::getParameters()
{
  const EmulsionFilmParameters * parameters = m_geometryManager->filmParameters();
  m_baseThickness = parameters->baseThickness();
  m_baseHeight    = parameters->baseHeight();
  m_baseWidth     = parameters->baseWidth();
  m_filmThickness = parameters->filmThickness();
  m_filmHeight    = parameters->filmHeight();
  m_filmWidth     = parameters->filmWidth();
  m_detectorManager->numerology().setNumFilmsPerBase(2);
}

const GeoLogVol * 
EmulsionBase::preBuild()
{
  // Create child elements
  m_baseboard = new EmulsionBaseboard("Baseboard", m_detectorManager, m_geometryManager, m_materials);
  m_front = new EmulsionFilm("FrontFilm", m_detectorManager, m_geometryManager, m_materials);
  m_back =  new EmulsionFilm("BackFilm", m_detectorManager, m_geometryManager, m_materials);

  // Build a box to hold the base and two films
  m_width = std::max(m_baseWidth, m_filmWidth);
  m_height = std::max(m_baseHeight, m_filmHeight);
  m_thickness = m_baseThickness + 2 * m_filmThickness;
  const GeoBox* baseShape = new GeoBox(0.5*m_width, 0.5*m_height, 0.5*m_thickness);

  GeoLogVol * baseLog = new GeoLogVol(getName(), baseShape, m_materials->gasMaterial());

  m_baseboardPos = new GeoTrf::Translate3D(0.0, 0.0, 0.0);
  m_frontPos     = new GeoTrf::Translate3D(0.0, 0.0, -(m_baseThickness + m_filmThickness)/2);
  m_backPos     = new GeoTrf::Translate3D(0.0, 0.0, (m_baseThickness + m_filmThickness)/2);

  return baseLog;
}

GeoVPhysVol * 
EmulsionBase::build(EmulsionIdentifier id)
{
  GeoFullPhysVol * base = new GeoFullPhysVol(m_logVolume); 

  // Add Baseboard
  GeoTransform * baseboardTransform = new GeoTransform(*m_baseboardPos);
  base->add(baseboardTransform);
  base->add(m_baseboard->getVolume());

  // Add front side
  GeoAlignableTransform * frontTransform = new GeoAlignableTransform(*m_frontPos);
  base->add(frontTransform);
  int frontNumber = 0;
  base->add(new GeoNameTag("Film#"+intToString(frontNumber))); // Identifier side=0
  base->add(new GeoIdentifierTag(frontNumber));
  id.setFilm(frontNumber);
  Identifier frontId = id.getFilmId();
  GeoVPhysVol * frontSide = m_front->build(id);
  base->add(frontSide);  
  // Store alignable transform
  m_detectorManager->addAlignableTransform(0, frontId, frontTransform, frontSide);

  // Add back side
  GeoAlignableTransform * backTransform = new GeoAlignableTransform(*m_backPos);
  base->add(backTransform);
  int backNumber = 1;
  base->add(new GeoNameTag("Film#"+intToString(backNumber))); // Identifier side=1
  base->add(new GeoIdentifierTag(backNumber));
  id.setFilm(backNumber);
  Identifier backId = id.getFilmId();
  GeoVPhysVol * backSide = m_back->build(id);
  base->add(backSide);  
  // Store alignable transform
  m_detectorManager->addAlignableTransform(0, backId, backTransform, backSide);
     
  return base;
}
