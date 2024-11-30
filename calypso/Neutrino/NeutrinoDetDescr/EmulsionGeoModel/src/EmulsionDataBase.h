/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionDataBase_H
#define EmulsionGeoModel_EmulsionDataBase_H

#include "EmulsionGeoModel/EmulsionGeoModelAthenaComps.h"
#include <string>

#include "RDBAccessSvc/IRDBAccessSvc.h"
class IRDBRecord;


class EmulsionDataBase
{

  
public:

  EmulsionDataBase(const EmulsionGeoModelAthenaComps* athenaComps);

  const EmulsionGeoModelAthenaComps* athenaComps() const;

  IRDBRecordset_ptr weightTable() const;
  IRDBRecordset_ptr scalingTable() const;
  IRDBRecordset_ptr topLevelTable() const;
  IRDBRecordset_ptr emulsionSupportTable() const;

  const IRDBRecord* emulsionGeneral() const;
  const IRDBRecord* emulsionFilm() const;
  const IRDBRecord* emulsionPlates() const;

  // Return the Emulsion version tag.
  const std::string & versionTag() const;

  MsgStream& msg (MSG::Level lvl) const;

private:
  
  EmulsionDataBase(const EmulsionDataBase &);
  EmulsionDataBase& operator= (const EmulsionDataBase &);

private:

  const EmulsionGeoModelAthenaComps* m_athenaComps;

  std::string m_emulsionVersionTag;

  IRDBRecordset_ptr m_weightTable;
  IRDBRecordset_ptr m_scalingTable;
  IRDBRecordset_ptr m_topLevel;
  IRDBRecordset_ptr m_conditions;

  IRDBRecordset_ptr m_emulsionGeneral;
  IRDBRecordset_ptr m_emulsionFilm;
  IRDBRecordset_ptr m_emulsionPlates;
  IRDBRecordset_ptr m_emulsionSupport;


};

#endif //EmulsionGeoModel_EmulsionDataBase_H
