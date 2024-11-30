/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "PreshowerGeoModel/PreshowerGeoModelAthenaComps.h"

#include <iostream>

PreshowerDataBase::PreshowerDataBase(const PreshowerGeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for Preshower
  DecodeFaserVersionKey versionKey(geoDbTag,"Preshower");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Scintillator.
  DecodeFaserVersionKey scintVersionKey(geoDbTag,"Scintillator");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // Preshower version tag
  m_preshowerVersionTag = rdbSvc->getChildTag("Preshower", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " Preshower Version:       " << m_preshowerVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // Preshower General
  //

  // Preshower TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("PreshowerTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("PreshowerWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("PreshowerMatScaling", scintVersionKey.tag(), scintVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerMatScaling Fetched" << endmsg;

//   // Default conditions
//   m_conditions = rdbSvc->getRecordsetPtr("PreshowerConditions", versionTag, versionNode, "FASERDD");
//   msg(MSG::DEBUG) << "Table PreshowerConditions Fetched" << endmsg;

  m_stationGeneral = rdbSvc->getRecordsetPtr("PreshowerStationGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerStationGeneral Fetched" << endmsg;

  m_plateGeneral = rdbSvc->getRecordsetPtr("PreshowerPlateGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerPlateGeneral Fetched" << endmsg;

  m_radiatorGeneral = rdbSvc->getRecordsetPtr("PreshowerRadiatorGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerRadiatorGeneral Fetched" << endmsg;

  m_absorberGeneral = rdbSvc->getRecordsetPtr("PreshowerAbsorberGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerAbsorberGeneral Fetched" << endmsg;

  m_wrappingGeneral = rdbSvc->getRecordsetPtr("PreshowerWrappingGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table PreshowerWrappingGeneral Fetched" << endmsg;

}

const PreshowerGeoModelAthenaComps* PreshowerDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr PreshowerDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr PreshowerDataBase::scalingTable() const {return m_scalingTable;}

// //const IRDBRecord* PreshowerDataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr PreshowerDataBase::topLevelTable() const {return m_topLevel;}

// IRDBRecordset_ptr PreshowerDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* PreshowerDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* PreshowerDataBase::stationGeneral() const {return (*m_stationGeneral)[0];}
const IRDBRecord* PreshowerDataBase::plateGeneral() const {return (*m_plateGeneral)[0];}
const IRDBRecord* PreshowerDataBase::radiatorGeneral() const {return (*m_radiatorGeneral)[0];}
const IRDBRecord* PreshowerDataBase::absorberGeneral() const {return (*m_absorberGeneral)[0];}
const IRDBRecord* PreshowerDataBase::wrappingGeneral() const {return (*m_wrappingGeneral)[0];}

const std::string & PreshowerDataBase::versionTag() const {
  return m_preshowerVersionTag;
}

MsgStream&  PreshowerDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
