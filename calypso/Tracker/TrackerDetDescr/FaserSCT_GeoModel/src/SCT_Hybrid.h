/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_Hybrid_H
#define FASERSCT_GEOMODEL_SCT_Hybrid_H

#include "SCT_ComponentFactory.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;

class SCT_Hybrid: public SCT_SharedComponentFactory

{
public:
  SCT_Hybrid(const std::string & name,
             TrackerDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}
  
private:
  void getParameters();
  virtual GeoVPhysVol * build();
  
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_length;

};

#endif // FASERSCT_GEOMODEL_SCT_Hybrid_H
