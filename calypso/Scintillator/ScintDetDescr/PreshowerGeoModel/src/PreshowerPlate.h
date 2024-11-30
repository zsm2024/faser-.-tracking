/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERPLATE_H
#define PRESHOWERGEOMODEL_PRESHOWERPLATE_H

#include "PreshowerComponentFactory.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;
namespace ScintDD{class ScintDetectorDesign;}

class PreshowerPlate: public PreshowerUniqueComponentFactory
{
public:
  PreshowerPlate(const std::string & name,
             ScintDD::PreshowerDetectorManager* detectorManager,
             const PreshowerGeometryManager* geometryManager,
             PreshowerMaterialManager* materials);

public:
  const GeoMaterial * material() const {return m_material;} 
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  virtual GeoVPhysVol * build(PreshowerIdentifier id);
  
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

#endif // PRESHOWERGEOMODEL_PRESHOWERPLATE_H
