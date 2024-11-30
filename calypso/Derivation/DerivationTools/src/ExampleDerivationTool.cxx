/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file ExamplederivationTool.cxx
 * Implementation file for the ExamplederivationTool class
 * @ author C. Gwilliam, 2021
 **/

#include "ExampleDerivationTool.h"

// Constructor
ExampleDerivationTool::ExampleDerivationTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
  //std::cout << "CTOR " << name << " with SaveFraction  = " << m_fraction << std::endl;
}

// Initialization
StatusCode
ExampleDerivationTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize()" );
  //std::cout << "INIT " << name() << " with SaveFraction  = " << m_fraction << std::endl;

  return StatusCode::SUCCESS;
}

bool
ExampleDerivationTool::passed(){

  bool accept(false);

  m_events++;

  float frac = ((float)(m_passed+1))/(float)m_events * 100.0;

  if (frac > m_fraction) {
    ATH_MSG_DEBUG("Filter failed " << m_passed << " " << m_events << " " << frac << " " << m_fraction);
    accept = false;
  } else {
    ATH_MSG_DEBUG("Filter passed " << m_passed << " " << m_events << " " << frac << " " << m_fraction);
    accept = true;
    m_passed++;
  }

  return accept;
}
