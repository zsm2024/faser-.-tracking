/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "VetoGeoModel/VetoGeoModelAthenaComps.h"

#include <iostream>

VetoDataBase::VetoDataBase(const VetoGeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for Veto
  DecodeFaserVersionKey versionKey(geoDbTag,"Veto");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Scintillator.
  DecodeFaserVersionKey scintVersionKey(geoDbTag,"Scintillator");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // Veto version tag
  m_vetoVersionTag = rdbSvc->getChildTag("Veto", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " Veto Version:       " << m_vetoVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // Veto General
  //

  // Veto TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("VetoTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("VetoWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("VetoMatScaling", scintVersionKey.tag(), scintVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table VetoMatScaling Fetched" << endmsg;

//   // Default conditions
//   m_conditions = rdbSvc->getRecordsetPtr("VetoConditions", versionTag, versionNode, "FASERDD");
//   msg(MSG::DEBUG) << "Table VetoConditions Fetched" << endmsg;

  m_stationGeneral = rdbSvc->getRecordsetPtr("VetoStationGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoStationGeneral Fetched" << endmsg;

  m_plateGeneral = rdbSvc->getRecordsetPtr("VetoPlateGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoPlateGeneral Fetched" << endmsg;

  m_radiatorGeneral = rdbSvc->getRecordsetPtr("VetoRadiatorGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoRadiatorGeneral Fetched" << endmsg;

  m_wrappingGeneral = rdbSvc->getRecordsetPtr("VetoWrappingGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoWrappingGeneral Fetched" << endmsg;


}

const VetoGeoModelAthenaComps* VetoDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr VetoDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr VetoDataBase::scalingTable() const {return m_scalingTable;}

// //const IRDBRecord* VetoDataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr VetoDataBase::topLevelTable() const {return m_topLevel;}

// IRDBRecordset_ptr VetoDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* VetoDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* VetoDataBase::stationGeneral() const {return (*m_stationGeneral)[0];}
const IRDBRecord* VetoDataBase::plateGeneral() const {return (*m_plateGeneral)[0];}
const IRDBRecord* VetoDataBase::radiatorGeneral() const {return (*m_radiatorGeneral)[0];}
const IRDBRecord* VetoDataBase::wrappingGeneral() const {return (*m_wrappingGeneral)[0];}

const std::string & VetoDataBase::versionTag() const {
  return m_vetoVersionTag;
}

MsgStream&  VetoDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
