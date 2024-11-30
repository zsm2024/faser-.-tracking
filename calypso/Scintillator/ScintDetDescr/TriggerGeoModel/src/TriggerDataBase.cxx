/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "TriggerGeoModel/TriggerGeoModelAthenaComps.h"

#include <iostream>

TriggerDataBase::TriggerDataBase(const TriggerGeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for Trigger
  DecodeFaserVersionKey versionKey(geoDbTag,"Trigger");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Scintillator.
  DecodeFaserVersionKey scintVersionKey(geoDbTag,"Scintillator");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // Trigger version tag
  m_triggerVersionTag = rdbSvc->getChildTag("Trigger", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " Trigger Version:       " << m_triggerVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // Trigger General
  //

  // Trigger TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("TriggerTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table TriggerTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("TriggerWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table TriggerWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("TriggerMatScaling", scintVersionKey.tag(), scintVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table TriggerMatScaling Fetched" << endmsg;

//   // Default conditions
//   m_conditions = rdbSvc->getRecordsetPtr("TriggerConditions", versionTag, versionNode, "FASERDD");
//   msg(MSG::DEBUG) << "Table TriggerConditions Fetched" << endmsg;

  m_stationGeneral = rdbSvc->getRecordsetPtr("TriggerStationGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table TriggerStationGeneral Fetched" << endmsg;

  m_plateGeneral = rdbSvc->getRecordsetPtr("TriggerPlateGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table TriggerPlateGeneral Fetched" << endmsg;

  m_wrappingGeneral = rdbSvc->getRecordsetPtr("TriggerWrappingGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table TriggerWrappingGeneral Fetched" << endmsg;


}

const TriggerGeoModelAthenaComps* TriggerDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr TriggerDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr TriggerDataBase::scalingTable() const {return m_scalingTable;}

// //const IRDBRecord* TriggerDataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr TriggerDataBase::topLevelTable() const {return m_topLevel;}

// IRDBRecordset_ptr TriggerDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* TriggerDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* TriggerDataBase::stationGeneral() const {return (*m_stationGeneral)[0];}
const IRDBRecord* TriggerDataBase::plateGeneral() const {return (*m_plateGeneral)[0];}
const IRDBRecord* TriggerDataBase::wrappingGeneral() const {return (*m_wrappingGeneral)[0];}

const std::string & TriggerDataBase::versionTag() const {
  return m_triggerVersionTag;
}

MsgStream&  TriggerDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
