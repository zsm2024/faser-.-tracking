/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_DETECTORFACTORY_H 
#define FASERSCT_GEOMODEL_SCT_DETECTORFACTORY_H 
 
#include "TrackerGeoModelUtils/TrackerDetectorFactoryBase.h" 
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/TrackerDD_Defs.h"

class GeoPhysVol;
class SCT_DataBase;
class SCT_GeometryManager;
class SCT_GeoModelAthenaComps;
class SCT_MaterialManager;
class SCT_Options;

class SCT_DetectorFactory : public TrackerDD::DetectorFactoryBase  
{ 
  
 public: 
  // Constructor
  SCT_DetectorFactory(const SCT_GeoModelAthenaComps * athenaComps, 
		      const SCT_Options & options); 

  // Destructor
  virtual ~SCT_DetectorFactory(); 

  // Creation of geometry:
  virtual void create(GeoPhysVol *world);   

  // Access to the results: 
  virtual const TrackerDD::SCT_DetectorManager * getDetectorManager() const; 

 private: 
  // Copy and assignments operations illegal and so are made private
  SCT_DetectorFactory(const SCT_DetectorFactory &right); 
  const SCT_DetectorFactory & operator=(const SCT_DetectorFactory &right); 

  TrackerDD::SCT_DetectorManager *m_detectorManager;
  SCT_GeometryManager *m_geometryManager;
  SCT_DataBase* m_db;
  SCT_MaterialManager* m_materials;
  bool m_useDynamicAlignFolders;

}; 
 
#endif 
 
