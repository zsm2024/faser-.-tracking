/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_HALFPLANE_H
#define FASERSCT_GEOMODEL_SCT_HALFPLANE_H

#include "SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;
class SCT_Module;
class SCT_Identifier;

class SCT_HalfPlane: public SCT_UniqueComponentFactory
{
public:
  SCT_HalfPlane(const std::string & name,
                TrackerDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);

  ~SCT_HalfPlane();
  //Explicitly disallow copy, assign to appease coverity
  SCT_HalfPlane(const SCT_HalfPlane &) = delete;
  SCT_HalfPlane & operator=(const SCT_HalfPlane &) = delete;

  virtual GeoVPhysVol* build(SCT_Identifier id) { return build(id, +1); }
  virtual GeoVPhysVol * build(SCT_Identifier id, int etaSign);


public:
  
  double width()  const      { return m_width; }         // y dimension of half-plane (local/module)
  double length() const      { return m_length; }        // z dimension of half-plane (local/module)
  double thickness() const   { return m_thickness; }     // x dimension of half-plane (local/module)

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  void activeEnvelopeExtent(double & dx, double & dy, double & dz);

  // Layer number
  double m_safety;
  double m_etaHalfPitch;            // horizontal global direction
  double m_phiHalfPitch;            // vertical global direction
  double m_depthHalfPitch;          // axial global direction
  
  double m_width;
  double m_length;
  double m_thickness;
  // double m_activeWidth;
  // double m_activeThickness;
  // double m_activeLength;

  SCT_Module*              m_module;
  const GeoLogVol*         m_logVolume;
};

#endif // FASERSCT_GEOMODEL_SCT_HALFPLANE_H
