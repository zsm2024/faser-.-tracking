/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GEOMODEL_SCT_WRAPPING_H
#define SCT_GEOMODEL_SCT_WRAPPING_H

#include "SCT_ComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class GeoMaterial;

class SCT_Wrapping : public SCT_SharedComponentFactory
{

public:
  SCT_Wrapping(const std::string & name,
               TrackerDD::SCT_DetectorManager* detectorManager,
               const SCT_GeometryManager* geometryManager,
               SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness()   const {return m_thickness;}
  double width()       const {return m_width;}
  double length()      const {return m_length;}
 
private:
  virtual GeoVPhysVol * build();
  void getParameters();
 
  const GeoMaterial * m_material;
  double      m_thickness;
  double      m_width;
  double      m_length;

  double      m_safety;
};

#endif // SCT_GEOMODEL_SCT_WRAPPING_H
