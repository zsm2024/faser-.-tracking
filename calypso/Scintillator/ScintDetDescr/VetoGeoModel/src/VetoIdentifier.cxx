/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "VetoIdentifier.h"
#include "ScintIdentifier/VetoID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
VetoIdentifier::getPlateId() 
{
  assert (m_idHelper);
  return m_idHelper->plate_id(m_station, m_plate);
}

void VetoIdentifier::print()
{
  std::cout << "2/1/" << m_station << "/" 
            << m_plate << std::endl;
}
