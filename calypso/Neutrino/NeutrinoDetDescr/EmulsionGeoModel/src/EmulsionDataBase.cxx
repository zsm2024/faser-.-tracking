/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionDataBase.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

#include "EmulsionGeoModel/EmulsionGeoModelAthenaComps.h"

#include <iostream>

EmulsionDataBase::EmulsionDataBase(const EmulsionGeoModelAthenaComps * athenaComps)
{
  m_athenaComps = athenaComps;

  IGeoDbTagSvc * geoDbTag = m_athenaComps->geoDbTagSvc();

  // Get version tag and node for Emulsion
  DecodeFaserVersionKey versionKey(geoDbTag,"Emulsion");
  std::string versionTag  = versionKey.tag();
  std::string versionNode = versionKey.node();

  // Get version tag and node for Neutrino.
  DecodeFaserVersionKey neutrinoVersionKey(geoDbTag,"Neutrino");

  // Access the RDB
  IRDBAccessSvc* rdbSvc = m_athenaComps->rdbAccessSvc();

  // Emulsion version tag
  m_emulsionVersionTag = rdbSvc->getChildTag("Emulsion", versionKey.tag(), versionKey.node(), "FASERDD");


/////////////////////////////////////////////////////////
//
// Gets the structures
//
/////////////////////////////////////////////////////////

    msg(MSG::INFO) << "Retrieving Record Sets from database ..." << endmsg;
    msg(MSG::DEBUG) << " Using version tag: " << versionTag << endmsg;
    msg(MSG::DEBUG) << "           at node: " << versionNode << endmsg;
    msg(MSG::DEBUG) << " Emulsion Version:       " << m_emulsionVersionTag << endmsg;

  // ATLS - not sure I use it.
  // General atlas parameters

  //
  // Emulsion General
  //

  // Emulsion TopLevel
  m_topLevel = rdbSvc->getRecordsetPtr("EmulsionTopLevel", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionTopLevel Fetched" << endmsg;

  // Weight Table
  m_weightTable = rdbSvc->getRecordsetPtr("EmulsionWeights", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionWeights Fetched" << endmsg;

  // Extra Scaling Table. This is used for extra material studies. For nominal material the table should be empty.
  // NB this is at InnerDetector level node.
  m_scalingTable = rdbSvc->getRecordsetPtr("EmulsionMatScaling", neutrinoVersionKey.tag(), neutrinoVersionKey.node(), "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionMatScaling Fetched" << endmsg;

//   // Default conditions
//   m_conditions = rdbSvc->getRecordsetPtr("EmulsionConditions", versionTag, versionNode, "FASERDD");
//   msg(MSG::DEBUG) << "Table EmulsionConditions Fetched" << endmsg;

  m_emulsionGeneral = rdbSvc->getRecordsetPtr("EmulsionGeneral", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionGeneral Fetched" << endmsg;

  m_emulsionFilm = rdbSvc->getRecordsetPtr("EmulsionFilm", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionFilm Fetched" << endmsg;

  m_emulsionPlates = rdbSvc->getRecordsetPtr("EmulsionPlates", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionPlates Fetched" << endmsg;

  m_emulsionSupport = rdbSvc->getRecordsetPtr("EmulsionSupport", versionTag, versionNode, "FASERDD");
  msg(MSG::DEBUG) << "Table EmulsionSupport Fetched" << endmsg;
}

const EmulsionGeoModelAthenaComps* EmulsionDataBase::athenaComps() const { return m_athenaComps; }

IRDBRecordset_ptr EmulsionDataBase::weightTable() const {return m_weightTable;}

IRDBRecordset_ptr EmulsionDataBase::scalingTable() const {return m_scalingTable;}

// //const IRDBRecord* EmulsionDataBase::atls() const {return *m_atls)[0];}  
IRDBRecordset_ptr EmulsionDataBase::topLevelTable() const {return m_topLevel;}

IRDBRecordset_ptr EmulsionDataBase::emulsionSupportTable() const {return m_emulsionSupport;}

// IRDBRecordset_ptr EmulsionDataBase::conditionsTable() const {return m_conditions;}
// const IRDBRecord* EmulsionDataBase::conditions() const {return (*m_conditions)[0];}

const IRDBRecord* EmulsionDataBase::emulsionGeneral() const {return (*m_emulsionGeneral)[0];}
const IRDBRecord* EmulsionDataBase::emulsionFilm() const {return (*m_emulsionFilm)[0];}
const IRDBRecord* EmulsionDataBase::emulsionPlates() const {return (*m_emulsionPlates)[0];}

const std::string & EmulsionDataBase::versionTag() const {
  return m_emulsionVersionTag;
}

MsgStream&  EmulsionDataBase::msg (MSG::Level lvl) const 
{ 
  return m_athenaComps->msg(lvl); 
}
