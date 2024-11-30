/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DipoleGeoModel/DipoleManager.h"

namespace TrackerDD {

DipoleManager::DipoleManager( ) 
{
  setName("Dipole");
}


DipoleManager::~DipoleManager()
{
  for(unsigned int i=0; i<m_volume.size(); i++)
    m_volume[i]->unref();
}



unsigned int 
DipoleManager::getNumTreeTops() const
{
  return m_volume.size(); 
}

PVConstLink 
DipoleManager::getTreeTop(unsigned int i) const
{
  return m_volume[i];
}

void 
DipoleManager::addTreeTop(PVLink vol){
  m_volume.push_back(vol);
  vol->ref();
}


} // namespace TrackerDD


