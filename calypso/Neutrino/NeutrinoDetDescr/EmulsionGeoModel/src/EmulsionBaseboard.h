/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONGEOMODEL_EMULSIONBASEBOARD_H
#define EMULSIONGEOMODEL_EMULSIONBASEBOARD_H

#include "EmulsionComponentFactory.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
namespace NeutrinoDD{class NeutrinoDetectorDesign;}

class EmulsionBaseboard: public EmulsionSharedComponentFactory
{
public:
  EmulsionBaseboard(const std::string & name,
             NeutrinoDD::EmulsionDetectorManager* detectorManager,
             const EmulsionGeometryManager* geometryManager,
             EmulsionMaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;} 
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double height()    const {return m_height;}

  virtual GeoVPhysVol * build();
  
private:
  void getParameters();
 
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_height;
  
};

#endif // EMULSIONGEOMODEL_EMULSIONBASEBOARD_H
