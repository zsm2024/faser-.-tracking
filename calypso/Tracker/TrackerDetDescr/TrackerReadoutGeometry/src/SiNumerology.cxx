/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerReadoutGeometry/SiNumerology.h"


namespace TrackerDD {

SiNumerology::SiNumerology()
  : m_numLayers(0),
    m_maxPhiCells(0),
    m_maxNumBarrelEta(0),
    m_maxNumBarrelPhiModules(0)
{}
  

void SiNumerology::setNumLayers(int nLayers)
{
  m_phiModulesForLayer.resize(nLayers);
  m_etaModulesForLayer.resize(nLayers);
  m_numLayers = nLayers;
}

void SiNumerology::setNumPhiModulesForLayer(int layer, int nPhiModules)
{
  m_phiModulesForLayer[layer] = nPhiModules;      
  m_maxNumBarrelPhiModules = std::max(m_maxNumBarrelPhiModules, nPhiModules);
}

void SiNumerology::setNumEtaModulesForLayer(int layer, int nEtaModules)
{
  m_etaModulesForLayer[layer] = nEtaModules;
  m_maxNumBarrelEta = std::max(m_maxNumBarrelEta, nEtaModules);
}

void SiNumerology::setMaxNumPhiCells(int cells)
{
  m_maxPhiCells = std::max(m_maxPhiCells,cells);
}

void SiNumerology::addBarrel(int id)
{
  m_barrelIds.push_back(id);
}
	
} // End namespace
