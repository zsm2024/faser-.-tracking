/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrenchDetectorManager_h
#define TrenchDetectorManager_h 1

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include <vector>

class TrenchDetectorManager : public GeoVDetectorManager  
{
 public:

  // Constructor
  TrenchDetectorManager();

  // Destructor
  ~TrenchDetectorManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink getTreeTop(unsigned int i) const;
 
  void addTreeTop(PVLink);      // Add a Tree top:

 private:  

  const TrenchDetectorManager & operator=(const TrenchDetectorManager &right);
  TrenchDetectorManager(const TrenchDetectorManager &right);
  
  // Tree Tops
  std::vector<PVLink> m_treeTops;
};

#ifndef GAUDI_NEUTRAL
#include "AthenaKernel/CLASS_DEF.h" 
CLASS_DEF(TrenchDetectorManager, 124400828, 1)
#endif

#endif


