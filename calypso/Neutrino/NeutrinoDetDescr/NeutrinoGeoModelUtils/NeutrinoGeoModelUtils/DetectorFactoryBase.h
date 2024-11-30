/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutrinoGeoModelUtils_DetectorFactoryBase_H
#define NeutrinoGeoModelUtils_DetectorFactoryBase_H

#include "GeoModelKernel/GeoVDetectorFactory.h" 
#include "NeutrinoGeoModelUtils/NeutrinoDDAthenaComps.h"

class StoreGateSvc;
class IGeoDbTagSvc;
class IRDBAccessSvc;

namespace NeutrinoDD {

class DetectorFactoryBase : public GeoVDetectorFactory
{ 

public:
  DetectorFactoryBase(const NeutrinoDD::AthenaComps * athenaComps)
    : m_athenaComps(athenaComps)
  {}

  StoreGateSvc * detStore() const {return m_athenaComps->detStore();}

  IGeoDbTagSvc * geoDbTagSvc() const {return m_athenaComps->geoDbTagSvc();}

  IRDBAccessSvc * rdbAccessSvc() const {return m_athenaComps->rdbAccessSvc();}
  
  IGeometryDBSvc * geomDB() const {return m_athenaComps->geomDB();}

 //Declaring the Message method for further use
  MsgStream& msg (MSG::Level lvl) const { return m_athenaComps->msg(lvl); }

  //Declaring the Method providing Verbosity Level
  bool msgLvl (MSG::Level lvl) { return m_athenaComps->msgLvl(lvl); }

  const NeutrinoDD::AthenaComps *  getAthenaComps() {return m_athenaComps;}

private:
  
  const NeutrinoDD::AthenaComps *  m_athenaComps;

};

} // end namespace

#endif // NeutrinoGeoModelUtils_DetectorFactoryBase_H

