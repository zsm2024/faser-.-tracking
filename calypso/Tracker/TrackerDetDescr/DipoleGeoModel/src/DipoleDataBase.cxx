/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DipoleGeoModel/DipoleDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "TrackerGeoModelUtils/TrackerDDAthenaComps.h"

#include <iostream>

DipoleDataBase::DipoleDataBase(const TrackerDD::AthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for Dipole
  DecodeFaserVersionKey versionKey(geoDbTag,"Dipole");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Tracker.
  DecodeFaserVersionKey trackerVersionKey(geoDbTag,"Tracker");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // Dipole version tag
  m_dipoleVersionTag = rdbSvc->getChildTag("Dipole", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " Dipole Version:       " << m_dipoleVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // Dipole General
  //

  // Dipole TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("DipoleTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table DipoleTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("DipoleWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table DipoleWeights Fetched" << endmsg;

  // // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // // NB this is at Tracker level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("DipoleMatScaling", trackerVersionKey.tag(), trackerVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table DipoleMatScaling Fetched" << endmsg;

  // // Default conditions
  // m_conditions = rdbSvc->getRecordsetPtr("DipoleConditions", versionTag, versionNode, "FASERDD");
  // msg(MSG::DEBUG) << "Table DipoleConditions Fetched" << endmsg;

  //
  // 
  //
  m_dipoleGeneral = rdbSvc->getRecordsetPtr("DipoleGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table DipoleGeneral Fetched" << endmsg;

  // Dipole Wrapping
  m_dipoleWrapping = rdbSvc->getRecordsetPtr("DipoleWrappingGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table DipoleWrappingGeneral Fetched" << endmsg;

}

const TrackerDD::AthenaComps* DipoleDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr DipoleDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr DipoleDataBase::scalingTable() const {return m_scalingTable;}

IRDBRecordset_ptr DipoleDataBase::topLevelTable() const {return m_topLevel;}

// IRDBRecordset_ptr DipoleDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* DipoleDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* DipoleDataBase::dipoleGeneral() const {return (*m_dipoleGeneral)[0];}

const IRDBRecord* DipoleDataBase::dipoleWrapping() const {return (*m_dipoleWrapping)[0];}

const std::string & DipoleDataBase::versionTag() const {
  return m_dipoleVersionTag;
}

MsgStream&  DipoleDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
