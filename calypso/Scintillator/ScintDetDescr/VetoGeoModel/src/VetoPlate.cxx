/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoPlate.h"

#include "VetoGeometryManager.h"
#include "VetoMaterialManager.h"

#include "VetoPlateParameters.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"

#include "ScintReadoutGeometry/VetoDetectorManager.h"
#include "ScintReadoutGeometry/ScintDetectorDesign.h"
#include "ScintReadoutGeometry/ScintDetectorElement.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"
#include "ScintReadoutGeometry/ScintCommonItems.h"

#include "GaudiKernel/SystemOfUnits.h"

using namespace ScintDD;

VetoPlate::VetoPlate(const std::string & name,
                     ScintDD::VetoDetectorManager* detectorManager,
                     const VetoGeometryManager* geometryManager,
                     VetoMaterialManager* materials)
  : VetoUniqueComponentFactory(name, detectorManager, geometryManager, materials),
    m_noElementWarning{true}
{
  getParameters();
  m_logVolume = preBuild();
}


void
VetoPlate::getParameters()
{
  
  const VetoPlateParameters * parameters = m_geometryManager->plateParameters();
  m_material  = m_materials->getMaterial(parameters->plateMaterial());
  m_thickness = parameters->plateThickness();
  m_length = parameters->plateLength();
  m_width     = parameters->plateWidth();
  m_detectorManager->numerology().setNumPmtsPerPlate(parameters->platePmts());
}

const GeoLogVol * 
VetoPlate::preBuild()
{

  // Build the plate. Just a simple box.
  // const GeoBox * plateShape = new GeoBox(0.5*m_thickness, 0.5*m_width, 0.5*m_length);
  const GeoBox * plateShape = new GeoBox(0.5*m_width, 0.5*m_length, 0.5*m_thickness);
  GeoLogVol * plateLog = new GeoLogVol(getName(), plateShape, m_material);

  // Make the scint design for this plate
  makeDesign();

  m_detectorManager->setDesign(m_design);
  
  return plateLog;
}


void
VetoPlate::makeDesign()
{
  //SiDetectorDesign::Axis etaAxis   = SiDetectorDesign::zAxis;
  //SiDetectorDesign::Axis phiAxis   = SiDetectorDesign::yAxis;
  //SiDetectorDesign::Axis depthAxis = SiDetectorDesign::xAxis;

  const VetoPlateParameters * parameters = m_geometryManager->plateParameters();
  
    m_design = new ScintDetectorDesign(m_thickness, 
                                       m_length, 
                                       m_width,
                                       parameters->platePmts());
}



GeoVPhysVol * 
VetoPlate::build(VetoIdentifier id)
{
  GeoFullPhysVol * plate = new GeoFullPhysVol(m_logVolume); 
  
  // Make detector element and add to collection
  // Only do so if we have a valid id helper.

  //id.print(); // for debugging only

  const ScintCommonItems* commonItems =  m_geometryManager->commonItems();

  if (commonItems->getIdHelper()) {

    ScintDetectorElement * detElement;

    detElement =  new ScintDetectorElement(id.getPlateId(), 
                                           m_design, 
                                           plate,  
                                           commonItems);
    
    // Add the detector element.
    m_detectorManager->addDetectorElement(detElement);

  } else {
    if (m_noElementWarning) {
      std::cout << "WARNING!!!!: No Veto id helper and so no elements being produced." << std::endl;
      m_noElementWarning = false;
    }
  }
  return plate;
}
