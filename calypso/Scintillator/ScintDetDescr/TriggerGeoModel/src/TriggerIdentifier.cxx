/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerIdentifier.h"
#include "ScintIdentifier/TriggerID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
TriggerIdentifier::getPlateId() 
{
  assert (m_idHelper);
  return m_idHelper->plate_id(m_station, m_plate);
}

void TriggerIdentifier::print()
{
  std::cout << "2/2/" << m_station << "/" 
            << m_plate << std::endl;
}
