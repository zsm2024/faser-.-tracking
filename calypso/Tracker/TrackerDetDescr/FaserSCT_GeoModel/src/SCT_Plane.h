/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//
#ifndef FASERSCT_GEOMODEL_SCT_PLANE_H
#define FASERSCT_GEOMODEL_SCT_PLANE_H

#include "SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class GeoLogVol;
class SCT_HalfPlane;
class SCT_Module;
class SCT_Identifier;

class SCT_Plane: public SCT_UniqueComponentFactory
{
public:
  SCT_Plane(const std::string & name,
            TrackerDD::SCT_DetectorManager* detectorManager,
            const SCT_GeometryManager* geometryManager,
            SCT_MaterialManager* materials);

  ~SCT_Plane();
  //Explicitly disallow copy, assign to appease coverity
  SCT_Plane(const SCT_Plane &) = delete;
  SCT_Plane & operator=(const SCT_Plane &) = delete;

  virtual GeoVPhysVol * build(SCT_Identifier id);


public:
  
  double width()  const   {return m_width;}         // x dimension of layer (global)
  double height() const   {return m_height;}        // y dimension of layer (global)
  double thickness() const   {return m_thickness;}  // z dimension of layer (global)

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();

  void activeEnvelopeExtent(double & dx, double & dy, double & dz);

  // Layer number
  double m_safety;
  
  double m_width;
  double m_height;
  double m_thickness;

  double m_sideHalfPitch;

  SCT_HalfPlane*      m_halfPlane;
  const GeoLogVol*    m_logVolume;
};

#endif // FASERSCT_GEOMODEL_SCT_PLANE_H
