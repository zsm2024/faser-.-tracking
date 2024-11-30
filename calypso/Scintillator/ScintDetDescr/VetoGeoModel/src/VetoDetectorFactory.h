/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETOGEOMODEL_VETODETECTORFACTORY_H 
#define VETOGEOMODEL_VETODETECTORFACTORY_H 
 
#include "ScintGeoModelUtils/DetectorFactoryBase.h" 
#include "ScintReadoutGeometry/VetoDetectorManager.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"

class GeoPhysVol;
class VetoDataBase;
class VetoGeometryManager;
class VetoGeoModelAthenaComps;
class VetoMaterialManager;
class VetoOptions;

class VetoDetectorFactory : public ScintDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  VetoDetectorFactory(const VetoGeoModelAthenaComps * athenaComps, 
		              const VetoOptions & options); 

  // Destructor
  virtual ~VetoDetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const ScintDD::VetoDetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  VetoDetectorFactory(const VetoDetectorFactory &right); 
  const VetoDetectorFactory & operator=(const VetoDetectorFactory &right); 

  ScintDD::VetoDetectorManager *m_detectorManager;
  VetoGeometryManager *m_geometryManager;
  VetoDataBase* m_db;
  VetoMaterialManager* m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
