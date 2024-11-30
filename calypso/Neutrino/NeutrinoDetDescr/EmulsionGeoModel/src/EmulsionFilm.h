/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONGEOMODEL_EMULSIONFILM_H
#define EMULSIONGEOMODEL_EMULSIONFILM_H

#include "EmulsionComponentFactory.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
namespace NeutrinoDD{class NeutrinoDetectorDesign;}

class EmulsionFilm: public EmulsionUniqueComponentFactory
{
public:
  EmulsionFilm(const std::string & name,
             NeutrinoDD::EmulsionDetectorManager* detectorManager,
             const EmulsionGeometryManager* geometryManager,
             EmulsionMaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;} 
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double height()    const {return m_height;}

  virtual GeoVPhysVol * build(EmulsionIdentifier id);
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void makeDesign();
 
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_height;
  
  NeutrinoDD::NeutrinoDetectorDesign* m_design;

  mutable std::atomic_bool m_noElementWarning;
};

#endif // EMULSIONGEOMODEL_EMULSIONFILM_H
