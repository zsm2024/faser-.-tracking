/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoNuIdentifier.h"
#include "ScintIdentifier/VetoNuID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
VetoNuIdentifier::getPlateId() 
{
  assert (m_idHelper);
  return m_idHelper->plate_id(m_station, m_plate);
}

void VetoNuIdentifier::print()
{
  std::cout << "2/1/" << m_station << "/" 
            << m_plate << std::endl;
}
