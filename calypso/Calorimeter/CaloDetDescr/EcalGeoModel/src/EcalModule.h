/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ECALGEOMODEL_ECALPLATE_H
#define ECALGEOMODEL_ECALPLATE_H

#include "EcalComponentFactory.h"
#include "GDMLInterface/GDMLController.h"

#include <atomic>
#include <string>

// class GeoMaterial;
class GeoVPhysVol;
namespace CaloDD{class CaloDetectorDesign;}

class EcalModule: public EcalUniqueComponentFactory
{
public:
  EcalModule(const std::string & name,
             CaloDD::EcalDetectorManager* detectorManager,
             const EcalGeometryManager* geometryManager,
             EcalMaterialManager* materials);

  ~EcalModule();

public:
  // const GeoMaterial * material() const {return m_material;} 
  double height() const {return m_height;}
  double width()     const {return m_width;}
  double length()    const {return m_length;}

  virtual GeoVPhysVol * build(EcalIdentifier id);
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
  void makeDesign(); 
 
  GDMLController m_controller {"GDMLController"};
  // const GeoMaterial * m_material;
  double m_width;
  double m_height;
  double m_length;
  
  const int m_pmtsPerModule {1};

  CaloDD::CaloDetectorDesign * m_design;

  mutable std::atomic_bool m_noElementWarning;
};

#endif // ECALGEOMODEL_ECALPLATE_H
