/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserEventInformation.h"

HepMC::GenEvent* FaserEventInformation::GetHepMCEvent()
{
	return m_theEvent;
}
void FaserEventInformation::SetHepMCEvent(HepMC::GenEvent* ev)
{
        m_theEvent=ev;
}
int FaserEventInformation::GetNrOfPrimaryParticles() const
{
	return m_nrOfPrimaryParticles;
}
void FaserEventInformation::SetNrOfPrimaryParticles(int nr)
{
	m_nrOfPrimaryParticles=nr;
}
int FaserEventInformation::GetNrOfPrimaryVertices() const
{
	return m_nrOfPrimaryVertices;
}
void FaserEventInformation::SetNrOfPrimaryVertices(int nr)
{
	m_nrOfPrimaryVertices=nr;
}
void FaserEventInformation::SetVertexPosition(const G4ThreeVector vtx)
{
	m_vertexPosition=vtx;
}
const G4ThreeVector FaserEventInformation::GetVertexPosition() const
{
	return m_vertexPosition;
}
