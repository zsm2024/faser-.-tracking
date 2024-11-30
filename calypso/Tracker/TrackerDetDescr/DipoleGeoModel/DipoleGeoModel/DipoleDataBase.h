/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DipoleGeoModel_DipoleDataBase_H
#define DipoleGeoModel_DipoleDataBase_H

#include "TrackerGeoModelUtils/TrackerDDAthenaComps.h"
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IRDBRecord;


class DipoleDataBase
{

  
public:

  DipoleDataBase(const TrackerDD::AthenaComps* athenaComps);

  const TrackerDD::AthenaComps* athenaComps() const;

  IRDBRecordset_ptr weightTable() const;
  IRDBRecordset_ptr scalingTable() const;
  IRDBRecordset_ptr topLevelTable() const;

  // IRDBRecordset_ptr conditionsTable() const;
  // const IRDBRecord* conditions() const;

  const IRDBRecord* dipoleGeneral() const;
  const IRDBRecord* dipoleWrapping() const;
  // Return the Dipole version tag.
  const std::string & versionTag() const;

  MsgStream& msg (MSG::Level lvl) const;

private:
  
  DipoleDataBase(const DipoleDataBase &);
  DipoleDataBase& operator= (const DipoleDataBase &);

private:

  const TrackerDD::AthenaComps* m_athenaComps;

  std::string m_dipoleVersionTag;

  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_scalingTable;
  IRDBRecordset_ptr m_topLevel;
  // IRDBRecordset_ptr m_conditions;

  IRDBRecordset_ptr m_dipoleGeneral;
  IRDBRecordset_ptr m_dipoleWrapping;

};

#endif //DipoleGeoModel_DipoleDataBase_H
