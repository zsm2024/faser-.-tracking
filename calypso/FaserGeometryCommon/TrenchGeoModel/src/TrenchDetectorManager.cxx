/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrenchDetectorManager.h"

TrenchDetectorManager::TrenchDetectorManager()
{
  setName("Trench");
}


TrenchDetectorManager::~TrenchDetectorManager()
{
  for(unsigned int i=0; i<m_treeTops.size(); i++)
    m_treeTops[i]->unref();
}


unsigned int TrenchDetectorManager::getNumTreeTops() const
{
  return m_treeTops.size();
}

PVConstLink TrenchDetectorManager::getTreeTop(unsigned int i) const
{
  if(i<m_treeTops.size())
    return m_treeTops[i];
  else
    return 0;
}

void  TrenchDetectorManager::addTreeTop(PVLink link) 
{
  link->ref();
  m_treeTops.push_back(link);
}

