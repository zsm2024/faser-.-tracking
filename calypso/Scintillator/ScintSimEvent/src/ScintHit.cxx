/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ScintSimEvent/ScintHit.h"
#include "ScintSimEvent/ScintHitIdHelper.h"

// Default consdtructor needed by athenaroot
//
ScintHit::ScintHit( ) :
  m_stX(0.),
  m_stY(0.),
  m_stZ(0.),
  m_enX(0.),
  m_enY(0.),
  m_enZ(0.),
  m_energyLoss(0.),
  m_meanTime(0.),
  m_partLink(),
  m_ID(0xffff)
{

}

// Constructor
ScintHit::ScintHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const int trackNumber,
                   const unsigned int id) :
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(trackNumber),
  m_ID(id)
{
}

// Constructor
ScintHit::ScintHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const int trackNumber,
                   const int veto_trigger_preshower, 
                   const int station, 
                   const int plate) :
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(trackNumber),
  m_ID(0)
{
  // Compress the location info into the integer:
  m_ID =  ScintHitIdHelper::GetHelper()->buildHitId(veto_trigger_preshower,
                                                    station, 
                                                    plate);
}

// Constructor
ScintHit::ScintHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const HepMcParticleLink &track,
                   const unsigned int id) :
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(track),
  m_ID(id)
{
}

// Constructor
ScintHit::ScintHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const HepMcParticleLink &track,
                   const int veto_trigger_preshower, 
                   const int station,
                   const int plate) : 
  m_stX( (float) localStartPosition.x() ),
  m_stY( (float) localStartPosition.y() ),
  m_stZ( (float) localStartPosition.z() ),
  m_enX( (float) localEndPosition.x() ),
  m_enY( (float) localEndPosition.y() ),
  m_enZ( (float) localEndPosition.z() ),
  m_energyLoss(energyLoss),
  m_meanTime(meanTime),
  m_partLink(track),
  m_ID(0)
{
  // Compress the location info into the integer:
  m_ID =  ScintHitIdHelper::GetHelper()->buildHitId(veto_trigger_preshower,
                                                    station,
                                                    plate);
}

void ScintHit::ScaleLength(double sfactor) {
  m_stX *=  (float) sfactor;
  m_stY *=  (float) sfactor;
  m_stZ *=  (float) sfactor;
  m_enX *=  (float) sfactor;
  m_enY *=  (float) sfactor;
  m_enZ *=  (float) sfactor;
}

bool ScintHit::isVeto() const {
  return  ScintHitIdHelper::GetHelper()->isVeto(m_ID);
}

bool ScintHit::isTrigger() const {
  return  ScintHitIdHelper::GetHelper()->isTrigger(m_ID);
}

bool ScintHit::isPreshower() const {
  return  ScintHitIdHelper::GetHelper()->isPreshower(m_ID);
}

bool ScintHit::isVetoNu() const {
  return ScintHitIdHelper::GetHelper()->isVetoNu(m_ID);
}

Identifier ScintHit::getIdentifier() const {
  return ScintHitIdHelper::GetHelper()->getIdentifier(m_ID);
}

HepGeom::Point3D<double> ScintHit::localStartPosition() const
{
  return HepGeom::Point3D<double>((double) m_stX, (double) m_stY, (double) m_stZ);
}

HepGeom::Point3D<double> ScintHit::localEndPosition() const
{
  return HepGeom::Point3D<double>((double) m_enX, (double) m_enY, (double) m_enZ);
}

int ScintHit::getStation() const {
  return  ScintHitIdHelper::GetHelper()->getStation(m_ID);
}

int ScintHit::getPlate() const {
  return  ScintHitIdHelper::GetHelper()->getPlate(m_ID);
}

void ScintHit::print() const {
  if (isVeto()) {
    std::cout << "*** Veto Hit " << std::endl;
  } else if (isTrigger()) {
    std::cout << "*** Trigger Hit " << std::endl;
  } else if (isPreshower()) {
    std::cout << "*** Preshower Hit " << std::endl; 
  } else if (isVetoNu()) {
    std::cout << "*** VetoNu Hit " << std::endl;
  } else {
      std::cout << "*** Unrecognized Scintillator Hit " << std::endl;
  }
  std::cout << "          Station Number " << getStation() << std::endl;
  std::cout << "          Plate Number   " << getPlate() << std::endl;
}

int ScintHit::trackNumber() const
{
  return m_partLink.barcode();
}
