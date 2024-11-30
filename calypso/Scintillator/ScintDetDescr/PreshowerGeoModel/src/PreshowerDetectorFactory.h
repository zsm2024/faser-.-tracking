/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERDETECTORFACTORY_H 
#define PRESHOWERGEOMODEL_PRESHOWERDETECTORFACTORY_H 
 
#include "ScintGeoModelUtils/DetectorFactoryBase.h" 
#include "ScintReadoutGeometry/PreshowerDetectorManager.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"

class GeoPhysVol;
class PreshowerDataBase;
class PreshowerGeometryManager;
class PreshowerGeoModelAthenaComps;
class PreshowerMaterialManager;
class PreshowerOptions;

class PreshowerDetectorFactory : public ScintDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  PreshowerDetectorFactory(const PreshowerGeoModelAthenaComps * athenaComps, 
		              const PreshowerOptions & options); 

  // Destructor
  virtual ~PreshowerDetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const ScintDD::PreshowerDetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  PreshowerDetectorFactory(const PreshowerDetectorFactory &right); 
  const PreshowerDetectorFactory & operator=(const PreshowerDetectorFactory &right); 

  ScintDD::PreshowerDetectorManager *m_detectorManager;
  PreshowerGeometryManager *m_geometryManager;
  PreshowerDataBase* m_db;
  PreshowerMaterialManager* m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
