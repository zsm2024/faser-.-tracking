/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERDETECTORFACTORY_H 
#define PRESHOWERGEOMODEL_PRESHOWERDETECTORFACTORY_H 
 
#include "CaloGeoModelUtils/DetectorFactoryBase.h" 
#include "CaloReadoutGeometry/EcalDetectorManager.h"
#include "CaloReadoutGeometry/CaloDD_Defs.h"

class GeoPhysVol;
class EcalDataBase;
class EcalGeometryManager;
class EcalGeoModelAthenaComps;
class EcalMaterialManager;
class EcalOptions;

class EcalDetectorFactory : public CaloDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  EcalDetectorFactory(const EcalGeoModelAthenaComps * athenaComps, 
		              const EcalOptions & options); 

  // Destructor
  virtual ~EcalDetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const CaloDD::EcalDetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  EcalDetectorFactory(const EcalDetectorFactory &right); 
  const EcalDetectorFactory & operator=(const EcalDetectorFactory &right); 

  CaloDD::EcalDetectorManager *m_detectorManager;
  EcalGeometryManager *m_geometryManager;
  EcalDataBase* m_db;
  EcalMaterialManager* m_materials;
  bool m_useDynamicAlignFolders;
  std::string m_gdmlFile;
}; 
 
#endif 
 
