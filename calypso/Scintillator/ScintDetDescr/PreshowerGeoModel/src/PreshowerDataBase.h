/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerDataBase_H
#define PreshowerGeoModel_PreshowerDataBase_H

#include "PreshowerGeoModel/PreshowerGeoModelAthenaComps.h"
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IRDBRecord;


class PreshowerDataBase
{

  
public:

  PreshowerDataBase(const PreshowerGeoModelAthenaComps* athenaComps);

  const PreshowerGeoModelAthenaComps* athenaComps() const;

  IRDBRecordset_ptr weightTable() const;
  IRDBRecordset_ptr scalingTable() const;
  IRDBRecordset_ptr topLevelTable() const;

  const IRDBRecord* stationGeneral() const;
  const IRDBRecord* plateGeneral() const;
  const IRDBRecord* radiatorGeneral() const;
  const IRDBRecord* absorberGeneral() const;
  const IRDBRecord* wrappingGeneral() const;

  // Return the Preshower version tag.
  const std::string & versionTag() const;

  MsgStream& msg (MSG::Level lvl) const;

private:
  
  PreshowerDataBase(const PreshowerDataBase &);
  PreshowerDataBase& operator= (const PreshowerDataBase &);

private:

  const PreshowerGeoModelAthenaComps* m_athenaComps;

  std::string m_preshowerVersionTag;

  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_scalingTable;
  IRDBRecordset_ptr m_topLevel;
  IRDBRecordset_ptr m_conditions;

  IRDBRecordset_ptr m_stationGeneral;
  IRDBRecordset_ptr m_plateGeneral;
  IRDBRecordset_ptr m_radiatorGeneral;
  IRDBRecordset_ptr m_absorberGeneral;
  IRDBRecordset_ptr m_wrappingGeneral;


};

#endif //PreshowerGeoModel_PreshowerDataBase_H
