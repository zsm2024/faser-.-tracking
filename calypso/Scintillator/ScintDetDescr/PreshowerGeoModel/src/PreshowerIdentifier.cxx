/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PreshowerIdentifier.h"
#include "ScintIdentifier/PreshowerID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
PreshowerIdentifier::getPlateId() 
{
  assert (m_idHelper);
  return m_idHelper->plate_id(m_station, m_plate);
}

void PreshowerIdentifier::print()
{
  std::cout << "2/3/" << m_station << "/" 
            << m_plate << std::endl;
}
