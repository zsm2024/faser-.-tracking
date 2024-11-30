/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionPlates.h"

#include "EmulsionGeometryManager.h"
#include "EmulsionMaterialManager.h"
#include "EmulsionGeneralParameters.h"
#include "EmulsionPlatesParameters.h"

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

EmulsionPlates::EmulsionPlates(const std::string & name,
                     NeutrinoDD::EmulsionDetectorManager* detectorManager,
                     const EmulsionGeometryManager* geometryManager,
                     EmulsionMaterialManager* materials)
  : EmulsionUniqueComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_logVolume = preBuild();
}

void
EmulsionPlates::getParameters()
{
  const EmulsionGeneralParameters * generalParameters = m_geometryManager->generalParameters();
  const EmulsionPlatesParameters *   platesParameters = m_geometryManager->platesParameters();
  const EmulsionSupportParameters*  supportParameters = m_geometryManager->supportParameters();

  m_tungstenWidth = platesParameters->platesWidth();
  m_tungstenHeight = platesParameters->platesHeight();
  m_tungstenThickness = platesParameters->platesThickness();
  m_absorberMaterial = m_materials->getMaterial(platesParameters->platesMaterial());
  m_airMaterial = m_materials->gasMaterial();

  m_nBasesPerModule = generalParameters->nBasesPerModule();
  m_nModules       = generalParameters->nModules();
  m_firstBaseZ     = generalParameters->firstBaseZ();
  m_lastBaseZ      = generalParameters->lastBaseZ();

  m_supportElements = supportParameters->supportElements();

  m_detectorManager->numerology().setNumBasesPerModule(m_nBasesPerModule);
}

const GeoLogVol * 
EmulsionPlates::preBuild()
{
  // create child element
  m_base = new EmulsionBase("Base", m_detectorManager, m_geometryManager, m_materials);

  double width = m_tungstenWidth/2;
  double height = m_tungstenHeight/2;
  // double thickness = m_tungstenThickness/2;
  double minZ = -m_tungstenThickness/2;
  double maxZ = m_tungstenThickness/2;

  for (auto e : m_supportElements)
  {
    width = std::max(width, std::max(std::abs(e.x + e.dx/2),std::abs(e.x - e.dx/2)));
    height = std::max(height, std::max(std::abs(e.y + e.dy/2),std::abs(e.y - e.dy/2)));
    minZ = std::min(minZ, e.z - e.dz/2);
    maxZ = std::max(maxZ, e.z + e.dz/2);
    const GeoBox* supportShape = new GeoBox(e.dx/2, e.dy/2, e.dz/2);
    GeoLogVol* supportLog = new GeoLogVol(e.label, supportShape, m_materials->getMaterial(e.mat));
    m_supportVolumes.push_back(supportLog);
  }

  m_width = 2 * width;
  m_height = 2 * height;
  m_thickness = maxZ - minZ;
  m_zShift = -(maxZ + minZ)/2;

  // Build a box to hold everything
  const GeoBox* platesShape = new GeoBox(0.5*m_width, 0.5*m_height, 0.5*m_thickness);

  // GeoLogVol * platesLog = new GeoLogVol(getName(), platesShape, m_materials->gasMaterial());
  GeoLogVol * platesLog = new GeoLogVol(getName(), platesShape, m_airMaterial);

  const GeoBox* absorberShape = new GeoBox(m_tungstenWidth/2, m_tungstenHeight/2, m_tungstenThickness/2);
  m_absorberVolume = new GeoLogVol(getName()+"_absorber", absorberShape, m_absorberMaterial);

  // m_baseboardPos = new GeoTrf::Translate3D(0.0, 0.0, 0.0);
  // m_frontPos     = new GeoTrf::Translate3D(0.0, 0.0, -(m_baseThickness + m_filmThickness)/2);
  // m_backPos     = new GeoTrf::Translate3D(0.0, 0.0, (m_baseThickness + m_filmThickness)/2);

  return platesLog;
}

GeoVPhysVol * 
EmulsionPlates::build(EmulsionIdentifier id)
{
  GeoFullPhysVol * plates = new GeoFullPhysVol(m_logVolume);
  GeoPhysVol * physAbsorber = new GeoPhysVol(m_absorberVolume);
  GeoAlignableTransform* absorberTransform = new GeoAlignableTransform( GeoTrf::Translate3D { 0.0, 0.0, m_zShift} );
  plates->add(absorberTransform);
  plates->add(new GeoNameTag("Absorber"));
  plates->add(physAbsorber);
  
  int nBases = 0;
  int nBasesTotal = m_nModules * m_nBasesPerModule;
  // Loop over modules
  for (int module = 0; module < m_nModules; module++)
  {
    id.setModule(module);
    // plates->add(new GeoNameTag("Module#"+intToString(module)));
    // plates->add(new GeoIdentifierTag(module));
    // Loop over bases in a module
    for (int base = 0; base < m_nBasesPerModule; base++)
    {
      id.setBase(base);
      // GeoAlignableTransform* theTransform = new GeoAlignableTransform( GeoTrf::Translate3D {0.0, 0.0, m_zShift + m_firstBaseZ + ((m_lastBaseZ - m_firstBaseZ)/(nBasesTotal-1))*nBases++} );
      // plates->add(theTransform);
      // plates->add(new GeoNameTag("Base#"+intToString(module*100 + base)));
      // plates->add(new GeoIdentifierTag(module*100 + base));
      // GeoVPhysVol* physBase = m_base->build(id);
      // plates->add(physBase);
      GeoAlignableTransform* theTransform = new GeoAlignableTransform( GeoTrf::Translate3D {0.0, 0.0, m_firstBaseZ + ((m_lastBaseZ - m_firstBaseZ)/(nBasesTotal-1))*nBases++} );
      physAbsorber->add(theTransform);
      physAbsorber->add(new GeoNameTag("Base#"+intToString(module*100 + base)));
      physAbsorber->add(new GeoIdentifierTag(module*100 + base));
      GeoVPhysVol* physBase = m_base->build(id);
      physAbsorber->add(physBase);
      m_detectorManager->addAlignableTransform(1, id.getFilmId(), theTransform, physBase);
    }
    m_detectorManager->numerology().useModule(module);
  }

  auto iv = m_supportVolumes.begin();
  for (auto e : m_supportElements)
  {
    GeoVPhysVol* pv = new GeoPhysVol(*iv);
    GeoTransform* theTransform = new GeoTransform( GeoTrf::Translate3D {e.x, e.y, e.z + m_zShift});
    plates->add(theTransform);
    plates->add(new GeoNameTag(e.label));
    plates->add(pv);
    iv++;
  }

  return plates;
}
