/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_DataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "FaserSCT_GeoModel/SCT_GeoModelAthenaComps.h"

#include <iostream>

SCT_DataBase::SCT_DataBase(const SCT_GeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for SCT
  DecodeFaserVersionKey versionKey(geoDbTag,"SCT");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Tracker.
  DecodeFaserVersionKey trackerVersionKey(geoDbTag,"Tracker");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // SCT version tag
  m_sctVersionTag = rdbSvc->getChildTag("SCT", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " SCT Version:       " << m_sctVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // SCT General
  //

  // SCT TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("SctTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("SctWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctWeights Fetched" << endmsg;

  // // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // // NB this is at Tracker level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("SctMatScaling", trackerVersionKey.tag(), trackerVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table SctMatScaling Fetched" << endmsg;

  // Detailed Frame Shape data (boolean volume components)
  m_frameShapeTable = rdbSvc->getRecordsetPtr("SctFrameShape", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctFrameShape Fetched" << endmsg;

  m_wrappingGeneral = rdbSvc->getRecordsetPtr("SctWrappingGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctWrappingGeneral Fetched" << endmsg;

  // // Default conditions
  // m_conditions = rdbSvc->getRecordsetPtr("SctConditions", versionTag, versionNode, "FASERDD");
  // msg(MSG::DEBUG) << "Table SctConditions Fetched" << endmsg;

  //
  // SCT Barrel 
  //
  m_brlSensor = rdbSvc->getRecordsetPtr("SctBrlSensor", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctBrlSensor Fetched" << endmsg;

  m_brlModule = rdbSvc->getRecordsetPtr("SctBrlModule", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctBrlModule Fetched" << endmsg;

  m_brlGeneral = rdbSvc->getRecordsetPtr("SctFaserGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctBrlGeneral Fetched" << endmsg;

  m_frameGeneral = rdbSvc->getRecordsetPtr("SctFrameGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table SctFrameGeneral Fetched" << endmsg;

}

const SCT_GeoModelAthenaComps* SCT_DataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr SCT_DataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr SCT_DataBase::scalingTable() const {return m_scalingTable;}

//const IRDBRecord* SCT_DataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr SCT_DataBase::topLevelTable() const {return m_topLevel;}

IRDBRecordset_ptr SCT_DataBase::frameShapeTable() const {return m_frameShapeTable;}

// IRDBRecordset_ptr SCT_DataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* SCT_DataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* SCT_DataBase::brlSensor() const {return (*m_brlSensor)[0];}
const IRDBRecord* SCT_DataBase::brlModule() const {return (*m_brlModule)[0];}

const IRDBRecord* SCT_DataBase::brlGeneral() const {return (*m_brlGeneral)[0];}
const IRDBRecord* SCT_DataBase::frameGeneral() const {return (*m_frameGeneral)[0];}
const IRDBRecord* SCT_DataBase::wrappingGeneral() const {return (*m_wrappingGeneral)[0];}

const std::string & SCT_DataBase::versionTag() const {
  return m_sctVersionTag;
}

MsgStream&  SCT_DataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
