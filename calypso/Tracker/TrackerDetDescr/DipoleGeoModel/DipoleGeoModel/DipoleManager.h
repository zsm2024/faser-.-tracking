/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This is an implemetation of the GeoVDetectorManager which
// is needed by Geo2G4 to get the treetop of the geoetry to 
// have the conversion to G4. Only the basic methods are implemented
//


#ifndef DIPOLEGEOMODEL_DIPOLEMANAGER_H
#define DIPOLEGEOMODEL_DIPOLEMANAGER_H

#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoVDetectorManager.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace TrackerDD {

class DipoleManager : public GeoVDetectorManager  {

 public:
  
  // Constructor
  DipoleManager( );

  // Destructor
  ~DipoleManager();

  // Access to raw geometry:
  virtual unsigned int getNumTreeTops() const;
  virtual PVConstLink getTreeTop(unsigned int i) const;

  // Add a Tree top:
  void addTreeTop(PVLink);

 private:  
  // prevent copy and assignment
  const DipoleManager & operator=(const DipoleManager &right);
  DipoleManager(const DipoleManager &right);

  // data members
  std::vector<PVLink> m_volume;
};

} // namespace TrackerDD

CLASS_DEF(TrackerDD::DipoleManager, 123016984, 1)

#endif





