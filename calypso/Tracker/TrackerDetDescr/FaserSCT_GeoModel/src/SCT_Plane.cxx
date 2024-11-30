/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
// CPW 17/8/06
// Version using new model of services from A. Tricoli
// 
#include "SCT_Plane.h"
#include "SCT_Identifier.h"
#include "SCT_GeometryManager.h"
#include "SCT_MaterialManager.h"
#include "SCT_BarrelParameters.h"
#include "SCT_GeneralParameters.h"
#include "SCT_HalfPlane.h"
#include "SCT_Module.h"

#include "TrackerGeoModelUtils/ExtraMaterial.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoShapeSubtraction.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <sstream>
#include <cmath>

inline double sqr(double x) {return x * x;}

SCT_Plane::SCT_Plane(const std::string & name,
                     TrackerDD::SCT_DetectorManager* detectorManager,
                     const SCT_GeometryManager* geometryManager,
                     SCT_MaterialManager* materials)
  : SCT_UniqueComponentFactory(name, detectorManager, geometryManager, materials)
{
  getParameters();
  m_logVolume = preBuild();
}

SCT_Plane::~SCT_Plane()
{
}

void
SCT_Plane::getParameters()
{
  const SCT_BarrelParameters * parameters = m_geometryManager->barrelParameters();
  const SCT_GeneralParameters * generalParameters = m_geometryManager->generalParameters();
    
  m_safety         = generalParameters->safety();
  m_sideHalfPitch  = parameters->sideHalfPitch();

}

const GeoLogVol * 
SCT_Plane::preBuild()
{
  m_halfPlane = new SCT_HalfPlane("HalfPlane", m_detectorManager, m_geometryManager, m_materials);
  //
  // Active Layer Envelope extent
  // Envelope for the active part (ie containing both half-planes for the layer)
  //
  double activeWidth, activeHeight, activeThickness;

  // Returns the outer dimensions of the active envelope
  activeEnvelopeExtent(activeWidth, activeHeight, activeThickness);

  // Add some safety
  activeWidth  += m_safety;
  activeHeight += m_safety;
  activeThickness += m_safety;

  //
  // Overall Layer Envelope extent
  //
  m_width = activeWidth;
  m_height = activeHeight;
  m_thickness = activeThickness;
  //
  // Make envelope for layer
  //
  const GeoBox* layerEnvelopeBox = new GeoBox(0.5 * m_width, 0.5 * m_height, 0.5 * m_thickness);
  GeoLogVol* logVolume = new GeoLogVol(getName(), layerEnvelopeBox, m_materials->gasMaterial());

  // Check for overlap.

  return logVolume;
}

GeoVPhysVol * 
SCT_Plane::build(SCT_Identifier id)
{
  // We make this a fullPhysVol
  GeoFullPhysVol * plane = new GeoFullPhysVol(m_logVolume);

  for (int side = 0; side < 2; side++)
  {
    plane->add(new GeoNameTag("Side#" + intToString(side)));
    plane->add(new GeoIdentifierTag(side));
    int iz = -1 + 2 * side;
    GeoTransform* transform = new GeoTransform(GeoTrf::Translate3D(0.0, 0.0, iz * m_sideHalfPitch) * 
                                               GeoTrf::RotateY3D(-iz * 90 * Gaudi::Units::deg));
    plane->add(transform);
    GeoVPhysVol* halfPlanePV = m_halfPlane->build(id, -iz);
    plane->add(halfPlanePV);
  }

  // Extra Material
  // TrackerDD::ExtraMaterial xMat(m_geometryManager->distortedMatManager());
  // xMat.add(layer, "SCTLayer");

  return plane;
}

void
SCT_Plane::activeEnvelopeExtent(double & dx, double & dy, double & dz)
{
  // Do this in a very simple-minded and unrealistic way to start
  double layerThickness = m_halfPlane->thickness() + 2 * m_sideHalfPitch + m_safety;// this is the x-coordinate of the modules and the z-coordinate of the plane
  double layerWidth = m_halfPlane->length() + m_safety;                             // this is the z-coordinate (eta) of the modules, and the x-coordinate of the plane
  double layerHeight = m_halfPlane->width() + m_safety;                             // this is the y-coordinate (phi) of the modules, and the y-coordinate of the plane

  dx = layerWidth;
  dy = layerHeight;
  dz = layerThickness;
}
