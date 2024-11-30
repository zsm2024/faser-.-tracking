/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "VetoNuGeoModel/VetoNuGeoModelAthenaComps.h"

#include <iostream>

VetoNuDataBase::VetoNuDataBase(const VetoNuGeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for VetoNu
  DecodeFaserVersionKey versionKey(geoDbTag,"VetoNu");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Scintillator.
  DecodeFaserVersionKey scintVersionKey(geoDbTag,"Scintillator");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // VetoNu version tag
  m_vetoVersionTag = rdbSvc->getChildTag("VetoNu", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " VetoNu Version:       " << m_vetoVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // VetoNu General
  //

  // VetoNu TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("VetoNuTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoNuTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("VetoNuWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoNuWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("VetoNuMatScaling", scintVersionKey.tag(), scintVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table VetoNuMatScaling Fetched" << endmsg;

//   // Default conditions
//   m_conditions = rdbSvc->getRecordsetPtr("VetoNuConditions", versionTag, versionNode, "FASERDD");
//   msg(MSG::DEBUG) << "Table VetoNuConditions Fetched" << endmsg;

  m_stationGeneral = rdbSvc->getRecordsetPtr("VetoNuStationGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoNuStationGeneral Fetched" << endmsg;

  m_plateGeneral = rdbSvc->getRecordsetPtr("VetoNuPlateGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoNuPlateGeneral Fetched" << endmsg;

  m_radiatorGeneral = rdbSvc->getRecordsetPtr("VetoNuRadiatorGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table VetoNuRadiatorGeneral Fetched" << endmsg;


}

const VetoNuGeoModelAthenaComps* VetoNuDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr VetoNuDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr VetoNuDataBase::scalingTable() const {return m_scalingTable;}

// //const IRDBRecord* VetoNuDataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr VetoNuDataBase::topLevelTable() const {return m_topLevel;}

// IRDBRecordset_ptr VetoNuDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* VetoNuDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* VetoNuDataBase::stationGeneral() const {return (*m_stationGeneral)[0];}
const IRDBRecord* VetoNuDataBase::plateGeneral() const {return (*m_plateGeneral)[0];}
const IRDBRecord* VetoNuDataBase::radiatorGeneral() const {return (*m_radiatorGeneral)[0];}

const std::string & VetoNuDataBase::versionTag() const {
  return m_vetoVersionTag;
}

MsgStream&  VetoNuDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
