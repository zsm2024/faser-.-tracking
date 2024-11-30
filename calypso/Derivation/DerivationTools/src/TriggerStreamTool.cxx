/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file TriggerStreamTool.cxx
 * Implementation file for the TriggerStreamTool class
 * @ author C. Gwilliam, 2021
 **/

#include "TriggerStreamTool.h"

// Constructor
TriggerStreamTool::TriggerStreamTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

// Initialization
StatusCode
TriggerStreamTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize() with mask = " << m_mask );

  ATH_CHECK( m_triggerDataKey.initialize() );

  return StatusCode::SUCCESS;
}

bool
TriggerStreamTool::passed() {

  SG::ReadHandle<xAOD::FaserTriggerData> triggerData(m_triggerDataKey);
  
  return triggerData->tap() & m_mask;
}
