/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutrinoReadoutGeometry/NeutrinoNumerology.h"


namespace NeutrinoDD {

NeutrinoNumerology::NeutrinoNumerology()
  : m_numBasesPerModule(0),
    m_numFilmsPerBase(0)
{}
  
void NeutrinoNumerology::addModule(int id)
{
  m_moduleIds.insert(id);
}

void NeutrinoNumerology::setNumBasesPerModule(int nBases)
{ 
  m_numBasesPerModule = nBases;
}

void NeutrinoNumerology::setNumFilmsPerBase(int nFilms)
{
  m_numFilmsPerBase = nFilms;
}

	
}  // End namespace
