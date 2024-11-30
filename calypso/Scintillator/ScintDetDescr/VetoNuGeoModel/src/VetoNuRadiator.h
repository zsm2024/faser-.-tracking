/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETONUGEOMODEL_VETONURADIATOR_H
#define VETONUGEOMODEL_VETONURADIATOR_H

#include "VetoNuComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class GeoMaterial;

class VetoNuRadiator : public VetoNuSharedComponentFactory
{

public:
  VetoNuRadiator(const std::string & name,
               ScintDD::VetoNuDetectorManager* detectorManager,
               const VetoNuGeometryManager* geometryManager,
               VetoNuMaterialManager* materials);

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

#endif // VETOGEOMODEL_VETORADIATOR_H
