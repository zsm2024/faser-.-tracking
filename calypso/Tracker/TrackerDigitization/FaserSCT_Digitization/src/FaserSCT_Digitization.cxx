/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_Digitization.h"
#include "PileUpTools/IPileUpTool.h"

//----------------------------------------------------------------------
// Constructor with parameters:
//----------------------------------------------------------------------
FaserSCT_Digitization::FaserSCT_Digitization(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator)
{
}

//----------------------------------------------------------------------
// Initialize method:
//----------------------------------------------------------------------
StatusCode FaserSCT_Digitization::initialize() {
  ATH_MSG_DEBUG("FaserSCT_Digitization::initialize()");
  return StatusCode::SUCCESS ;
}

//----------------------------------------------------------------------
// Execute method:
//----------------------------------------------------------------------

StatusCode FaserSCT_Digitization::execute() {
  ATH_MSG_DEBUG("execute()");
  return m_sctDigitizationTool->processAllSubEvents(Gaudi::Hive::currentContext());
}
