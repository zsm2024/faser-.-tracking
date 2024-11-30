/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERGEOMODEL_TRIGGERDETECTORFACTORY_H 
#define TRIGGERGEOMODEL_TRIGGERDETECTORFACTORY_H 
 
#include "ScintGeoModelUtils/DetectorFactoryBase.h" 
#include "ScintReadoutGeometry/TriggerDetectorManager.h"
#include "ScintReadoutGeometry/ScintDD_Defs.h"

class GeoPhysVol;
class TriggerDataBase;
class TriggerGeometryManager;
class TriggerGeoModelAthenaComps;
class TriggerMaterialManager;
class TriggerOptions;

class TriggerDetectorFactory : public ScintDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  TriggerDetectorFactory(const TriggerGeoModelAthenaComps * athenaComps, 
		              const TriggerOptions & options); 

  // Destructor
  virtual ~TriggerDetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const ScintDD::TriggerDetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  TriggerDetectorFactory(const TriggerDetectorFactory &right); 
  const TriggerDetectorFactory & operator=(const TriggerDetectorFactory &right); 

  ScintDD::TriggerDetectorManager *m_detectorManager;
  TriggerGeometryManager *m_geometryManager;
  TriggerDataBase* m_db;
  TriggerMaterialManager* m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
