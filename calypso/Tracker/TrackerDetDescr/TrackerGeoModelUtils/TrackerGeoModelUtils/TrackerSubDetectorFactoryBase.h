/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackerGeoModelUtils_SubDetectorFactoryBase_H
#define TrackerGeoModelUtils_SubDetectorFactoryBase_H

#include "AthenaBaseComps/AthMessaging.h"
#include "TrackerGeoModelUtils/TrackerDDAthenaComps.h"

class StoreGateSvc;
class IGeoDbTagSvc;
class IRDBAccessSvc;
class TrackerMaterialManager;

namespace TrackerDD {

// This is the same as Tracker::DetectorFactoryBase but without the
// inheretance of GeoVDetectorFactory and with the addition of 
// access to the material manager.

class SubDetectorFactoryBase : public AthMessaging
{ 

public:
  SubDetectorFactoryBase(const TrackerDD::AthenaComps * athenaComps)
    : m_athenaComps(athenaComps),
      m_materialManager(0)
  {}

  SubDetectorFactoryBase(const TrackerDD::AthenaComps * athenaComps,
			 TrackerMaterialManager * matManager)
    : m_athenaComps(athenaComps),
      m_materialManager(matManager)
  {}

  StoreGateSvc * detStore() const {return m_athenaComps->detStore();}

  IGeoDbTagSvc * geoDbTagSvc() const {return m_athenaComps->geoDbTagSvc();}

  IRDBAccessSvc * rdbAccessSvc() const {return m_athenaComps->rdbAccessSvc();}
  
  IGeometryDBSvc * geomDB() const {return m_athenaComps->geomDB();}

  TrackerMaterialManager * materialManager() const {return m_materialManager;}

  const TrackerDD::AthenaComps *  getAthenaComps() {return m_athenaComps;}
  
private:
  const TrackerDD::AthenaComps *  m_athenaComps;
  
protected:
  TrackerMaterialManager * m_materialManager;
};

} // end namespace

#endif // TrackerGeoModelUtils_SubDetectorFactoryBase_H

