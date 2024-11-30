/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "EcalGeoModel/EcalGeoModelAthenaComps.h"

#include <iostream>

EcalDataBase::EcalDataBase(const EcalGeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for Ecal
  DecodeFaserVersionKey versionKey(geoDbTag,"Ecal");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Calorimeter.
  DecodeFaserVersionKey caloVersionKey(geoDbTag,"Calorimeter");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // Ecal version tag
  m_ecalVersionTag = rdbSvc->getChildTag("Ecal", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " Ecal Version:       " << m_ecalVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // Ecal General
  //

  // Ecal TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("EcalTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EcalTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("EcalWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EcalWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("EcalMatScaling", caloVersionKey.tag(), caloVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table EcalMatScaling Fetched" << endmsg;

//   // Default conditions
//   m_conditions = rdbSvc->getRecordsetPtr("EcalConditions", versionTag, versionNode, "FASERDD");
//   msg(MSG::DEBUG) << "Table EcalConditions Fetched" << endmsg;

  m_rowGeneral = rdbSvc->getRecordsetPtr("EcalRowGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EcalRowGeneral Fetched" << endmsg;

  // m_plateGeneral = rdbSvc->getRecordsetPtr("EcalPlateGeneral", versionTag, versionNode, "FASERDD");
  // msg(MSG::DEBUG) << "Table EcalPlateGeneral Fetched" << endmsg;

}

const EcalGeoModelAthenaComps* EcalDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr EcalDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr EcalDataBase::scalingTable() const {return m_scalingTable;}

// //const IRDBRecord* EcalDataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr EcalDataBase::topLevelTable() const {return m_topLevel;}

// IRDBRecordset_ptr EcalDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* EcalDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* EcalDataBase::rowGeneral() const {return (*m_rowGeneral)[0];}
// const IRDBRecord* EcalDataBase::plateGeneral() const {return (*m_plateGeneral)[0];}

const std::string & EcalDataBase::versionTag() const {
  return m_ecalVersionTag;
}

MsgStream&  EcalDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
