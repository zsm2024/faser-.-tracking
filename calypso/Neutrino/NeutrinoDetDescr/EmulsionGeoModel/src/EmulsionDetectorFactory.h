/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOGEOMODEL_NEUTRINODETECTORFACTORY_H 
#define NEUTRINOGEOMODEL_NEUTRINODETECTORFACTORY_H 
 
#include "NeutrinoGeoModelUtils/DetectorFactoryBase.h" 
#include "NeutrinoReadoutGeometry/EmulsionDetectorManager.h"
#include "NeutrinoReadoutGeometry/NeutrinoDD_Defs.h"

class GeoPhysVol;
class EmulsionDataBase;
class EmulsionGeometryManager;
class EmulsionGeoModelAthenaComps;
class EmulsionMaterialManager;
class EmulsionOptions;

class EmulsionDetectorFactory : public NeutrinoDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  EmulsionDetectorFactory(const EmulsionGeoModelAthenaComps * athenaComps, 
		              const EmulsionOptions & options); 

  // Destructor
  virtual ~EmulsionDetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const NeutrinoDD::EmulsionDetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  EmulsionDetectorFactory(const EmulsionDetectorFactory &right); 
  const EmulsionDetectorFactory & operator=(const EmulsionDetectorFactory &right); 

  NeutrinoDD::EmulsionDetectorManager *m_detectorManager;
  EmulsionGeometryManager *m_geometryManager;
  EmulsionDataBase* m_db;
  EmulsionMaterialManager* m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
