/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETONUGEOMODEL_VETONUDETECTORFACTORY_H 
#define VETONUGEOMODEL_VETONUDETECTORFACTORY_H 
 
#include "ScintGeoModelUtils/DetectorFactoryBase.h" 
#include "ScintReadoutGeometry/VetoNuDetectorManager.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"

class GeoPhysVol;
class VetoNuDataBase;
class VetoNuGeometryManager;
class VetoNuGeoModelAthenaComps;
class VetoNuMaterialManager;
class VetoNuOptions;

class VetoNuDetectorFactory : public ScintDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  VetoNuDetectorFactory(const VetoNuGeoModelAthenaComps * athenaComps, 
		              const VetoNuOptions & options); 

  // Destructor
  virtual ~VetoNuDetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const ScintDD::VetoNuDetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  VetoNuDetectorFactory(const VetoNuDetectorFactory &right); 
  const VetoNuDetectorFactory & operator=(const VetoNuDetectorFactory &right); 

  ScintDD::VetoNuDetectorManager *m_detectorManager;
  VetoNuGeometryManager *m_geometryManager;
  VetoNuDataBase* m_db;
  VetoNuMaterialManager* m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
