/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrenchDetectorFactory_h
#define TrenchDetectorFactory_h 1

#include "GeoModelKernel/GeoVDetectorFactory.h"
#include "TrenchDetectorManager.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "GDMLInterface/GDMLController.h"

#include <string>

class StoreGateSvc;

class TrenchDetectorFactory : public GeoVDetectorFactory  
{
 
 public:
  
  // Constructor:
  TrenchDetectorFactory(StoreGateSvc *pDetStore,
			  IRDBAccessSvc *pAccess);
  
  // Destructor:
  ~TrenchDetectorFactory();
  
  // Creation of geometry:
  virtual void create(GeoPhysVol *world);
  
  // Access to the results:
  virtual const TrenchDetectorManager * getDetectorManager() const;
  
  // Set version Tag and Node
  void setTagNode(const std::string& tag, const std::string& node);

 
 private:  
  // Illegal operations:
  const TrenchDetectorFactory & operator=(const TrenchDetectorFactory &right);
  TrenchDetectorFactory(const TrenchDetectorFactory &right);
  
  // The manager:
  TrenchDetectorManager       *m_detectorManager;
  
  StoreGateSvc             *m_detectorStore;
  IRDBAccessSvc            *m_access;
  std::string              m_versionTag;
  std::string              m_versionNode;

//   GDMLController m_controller {"TrenchGDMLController"};

};

// Class TrenchDetectorFactory
#endif


