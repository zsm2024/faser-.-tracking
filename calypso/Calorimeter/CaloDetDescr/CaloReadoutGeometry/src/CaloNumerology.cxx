/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloReadoutGeometry/CaloNumerology.h"


namespace CaloDD {

CaloNumerology::CaloNumerology()
  : m_numModulesPerRow(0),
    m_numPmtsPerModule(0)
{}
  
void CaloNumerology::addRow(int id)
{
  m_rowIds.insert(id);
}

void CaloNumerology::setNumModulesPerRow(int nModules)
{ 
  m_numModulesPerRow = nModules;
}

void CaloNumerology::setNumPmtsPerModule(int nPmts)
{
  m_numPmtsPerModule = nPmts;
}

	
}  // End namespace
