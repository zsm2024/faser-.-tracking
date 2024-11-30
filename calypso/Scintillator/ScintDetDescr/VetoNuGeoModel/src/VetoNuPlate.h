/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETONUGEOMODEL_VETONUPLATE_H
#define VETONUGEOMODEL_VETONUPLATE_H

#include "VetoNuComponentFactory.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
namespace ScintDD{class ScintDetectorDesign;}

class VetoNuPlate: public VetoNuUniqueComponentFactory
{
public:
  VetoNuPlate(const std::string & name,
             ScintDD::VetoNuDetectorManager* detectorManager,
             const VetoNuGeometryManager* geometryManager,
             VetoNuMaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;} 
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  virtual GeoVPhysVol * build(VetoNuIdentifier id);
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void makeDesign(); 
 
  const GeoMaterial * m_material;
  double m_thickness;
  double m_width;
  double m_length;
  
  ScintDD::ScintDetectorDesign * m_design;

  mutable std::atomic_bool m_noElementWarning;
};

#endif // VETOGEOMODEL_VETOPLATE_H
