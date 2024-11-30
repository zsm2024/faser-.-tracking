/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NeutrinoGeoModelUtils_NeutrinoDDAthenaComps_H
#define NeutrinoGeoModelUtils_NeutrinoDDAthenaComps_H

// Message Stream Member
#include "AthenaBaseComps/AthMessaging.h"
class  StoreGateSvc;
class  IGeoDbTagSvc;
class  IRDBAccessSvc;
class  IGeometryDBSvc; 

#include <string>
namespace NeutrinoDD {

/// Class to hold various Athena components.
class AthenaComps : public AthMessaging
{

public:

  AthenaComps(const std::string & msgStreamName);

  void setDetStore(StoreGateSvc *);
  void setGeoDbTagSvc(IGeoDbTagSvc *);
  void setRDBAccessSvc(IRDBAccessSvc *);
  void setGeometryDBSvc(IGeometryDBSvc *);

  StoreGateSvc * detStore() const;
  IGeoDbTagSvc * geoDbTagSvc() const;
  IRDBAccessSvc * rdbAccessSvc() const;
  IGeometryDBSvc * geomDB() const;
  
private:
  StoreGateSvc * m_detStore;
  IGeoDbTagSvc * m_geoDbTagSvc;
  IRDBAccessSvc * m_rdbAccessSvc;
  IGeometryDBSvc * m_geometryDBSvc;

};

inline StoreGateSvc * AthenaComps::detStore() const
{
  return m_detStore;
}

inline IGeoDbTagSvc * AthenaComps::geoDbTagSvc() const
{
  return m_geoDbTagSvc;
}

inline IRDBAccessSvc * AthenaComps::rdbAccessSvc() const
{
  return m_rdbAccessSvc;
}

inline IGeometryDBSvc * AthenaComps::geomDB() const
{
  return m_geometryDBSvc;
}

} // endnamespace

#endif // NeutrinoGeoModelUtils_NeutrinoDDAthenaComps_H


