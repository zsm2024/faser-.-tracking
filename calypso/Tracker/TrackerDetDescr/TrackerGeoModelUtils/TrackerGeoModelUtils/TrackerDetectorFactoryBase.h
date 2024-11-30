/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrackerGeoModelUtils_DetectorFactoryBase_H
#define TrackerGeoModelUtils_DetectorFactoryBase_H

#include "GeoModelKernel/GeoVDetectorFactory.h" 
#include "TrackerGeoModelUtils/TrackerDDAthenaComps.h"
#include <utility>

class StoreGateSvc;
class IGeoDbTagSvc;
class IRDBAccessSvc;

namespace TrackerDD {

class DetectorFactoryBase : public GeoVDetectorFactory
{ 

public:
  DetectorFactoryBase(const TrackerDD::AthenaComps * athenaComps)
    : m_athenaComps(athenaComps)
  {}

  StoreGateSvc * detStore() const {return m_athenaComps->detStore();}

  IGeoDbTagSvc * geoDbTagSvc() const {return m_athenaComps->geoDbTagSvc();}

  IRDBAccessSvc * rdbAccessSvc() const {return m_athenaComps->rdbAccessSvc();}
  
  IGeometryDBSvc * geomDB() const {return m_athenaComps->geomDB();}

  const TrackerDD::AthenaComps *  getAthenaComps() {return m_athenaComps;}

 //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) { return m_athenaComps->msgLvl(lvl); }

private:
  
  const TrackerDD::AthenaComps *  m_athenaComps;

};

} // end namespace

#endif // TrackerGeoModelUtils_DetectorFactoryBase_H

