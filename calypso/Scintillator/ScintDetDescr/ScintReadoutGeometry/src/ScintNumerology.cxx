/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ScintReadoutGeometry/ScintNumerology.h"


namespace ScintDD {

ScintNumerology::ScintNumerology()
  : m_numPlatesPerStation(0),
    m_numPmtsPerPlate(0)
{}
  
void ScintNumerology::addStation(int id)
{
  m_stationIds.insert(id);
}

void ScintNumerology::setNumPlatesPerStation(int nPlates)
{ 
  m_numPlatesPerStation = nPlates;
}

void ScintNumerology::setNumPmtsPerPlate(int nPmts)
{
  m_numPmtsPerPlate = nPmts;
}

	
}  // End namespace
