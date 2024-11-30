/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserG4EventUserInfo.h"

HepMC::GenEvent* FaserG4EventUserInfo::GetHepMCEvent()
{
	return m_theEvent;
}
void FaserG4EventUserInfo::SetHepMCEvent(HepMC::GenEvent* ev)
{
        m_theEvent=ev;
}
int FaserG4EventUserInfo::GetNrOfPrimaryParticles() const
{
	return m_nrOfPrimaryParticles;
}
void FaserG4EventUserInfo::SetNrOfPrimaryParticles(int nr)
{
	m_nrOfPrimaryParticles=nr;
}
int FaserG4EventUserInfo::GetNrOfPrimaryVertices() const
{
	return m_nrOfPrimaryVertices;
}
void FaserG4EventUserInfo::SetNrOfPrimaryVertices(int nr)
{
	m_nrOfPrimaryVertices=nr;
}
void FaserG4EventUserInfo::SetVertexPosition(const G4ThreeVector& vtx)
{
	m_vertexPosition=vtx;
}
const G4ThreeVector FaserG4EventUserInfo::GetVertexPosition() const
{
	return m_vertexPosition;
}

