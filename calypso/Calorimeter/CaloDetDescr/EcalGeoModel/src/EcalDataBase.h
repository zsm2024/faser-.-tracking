/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EcalGeoModel_EcalDataBase_H
#define EcalGeoModel_EcalDataBase_H

#include "EcalGeoModel/EcalGeoModelAthenaComps.h"
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IRDBRecord;


class EcalDataBase
{

  
public:

  EcalDataBase(const EcalGeoModelAthenaComps* athenaComps);

  const EcalGeoModelAthenaComps* athenaComps() const;

  IRDBRecordset_ptr weightTable() const;
  IRDBRecordset_ptr scalingTable() const;
  IRDBRecordset_ptr topLevelTable() const;

  const IRDBRecord* rowGeneral() const;
  // const IRDBRecord* plateGeneral() const;

  // Return the Ecal version tag.
  const std::string & versionTag() const;

  MsgStream& msg (MSG::Level lvl) const;

private:
  
  EcalDataBase(const EcalDataBase &);
  EcalDataBase& operator= (const EcalDataBase &);

private:

  const EcalGeoModelAthenaComps* m_athenaComps;

  std::string m_ecalVersionTag;

  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_scalingTable;
  IRDBRecordset_ptr m_topLevel;
  IRDBRecordset_ptr m_conditions;

  IRDBRecordset_ptr m_rowGeneral;
  // IRDBRecordset_ptr m_plateGeneral;


};

#endif //EcalGeoModel_EcalDataBase_H
