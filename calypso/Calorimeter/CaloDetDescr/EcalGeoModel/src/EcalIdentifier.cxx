/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalIdentifier.h"
#include "FaserCaloIdentifier/EcalID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
EcalIdentifier::getModuleId() 
{
  assert (m_idHelper);
  return m_idHelper->module_id(m_row, m_module);
}

void EcalIdentifier::print()
{
  std::cout << "4/1/" << m_row << "/" 
            << m_module << std::endl;
}
