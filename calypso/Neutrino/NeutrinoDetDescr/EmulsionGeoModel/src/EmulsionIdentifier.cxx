/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionIdentifier.h"
#include "NeutrinoIdentifier/EmulsionID.h"
#include "Identifier/Identifier.h"

#include <cassert>
#include <iostream>

Identifier 
EmulsionIdentifier::getFilmId() 
{
  assert (m_idHelper);
  return m_idHelper->film_id(m_module, m_base, m_film);
}

void EmulsionIdentifier::print()
{
  std::cout << "1/1/" << m_module << "/" 
            << m_base << "/" << m_film << std::endl;
}
