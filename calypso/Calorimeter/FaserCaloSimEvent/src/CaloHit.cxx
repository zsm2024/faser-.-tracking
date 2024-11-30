/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserCaloSimEvent/CaloHit.h"
#include "FaserCaloSimEvent/CaloHitIdHelper.h"

// Default consdtructor needed by athenaroot
//
CaloHit::CaloHit( ) :
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
CaloHit::CaloHit(const HepGeom::Point3D<double> &localStartPosition,
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
CaloHit::CaloHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const int trackNumber,
                  //  const int veto_trigger_preshower, 
                   const int row, 
                   const int module) :
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
  m_ID =  CaloHitIdHelper::GetHelper()->buildHitId( row, 
                                                    module);
}

// Constructor
CaloHit::CaloHit(const HepGeom::Point3D<double> &localStartPosition,
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
CaloHit::CaloHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const HepMcParticleLink &track,
                  //  const int veto_trigger_preshower, 
                   const int row,
                   const int module) : 
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
  m_ID =  CaloHitIdHelper::GetHelper()->buildHitId( row,
                                                    module);
}

void CaloHit::ScaleLength(double sfactor) {
  m_stX *=  (float) sfactor;
  m_stY *=  (float) sfactor;
  m_stZ *=  (float) sfactor;
  m_enX *=  (float) sfactor;
  m_enY *=  (float) sfactor;
  m_enZ *=  (float) sfactor;
}

HepGeom::Point3D<double> CaloHit::localStartPosition() const
{
  return HepGeom::Point3D<double>((double) m_stX, (double) m_stY, (double) m_stZ);
}

HepGeom::Point3D<double> CaloHit::localEndPosition() const
{
  return HepGeom::Point3D<double>((double) m_enX, (double) m_enY, (double) m_enZ);
}

int CaloHit::getRow() const {
  return  CaloHitIdHelper::GetHelper()->getRow(m_ID);
}

int CaloHit::getModule() const {
  return  CaloHitIdHelper::GetHelper()->getModule(m_ID);
}

Identifier CaloHit::getIdentifier() const {
  return CaloHitIdHelper::GetHelper()->getIdentifier(m_ID);
}

void CaloHit::print() const {
  std::cout << "*** Calorimeter Hit" << std::endl;
  std::cout << "          Station Number " << getRow() << std::endl;
  std::cout << "          Plate Number   " << getModule() << std::endl;
}

int CaloHit::trackNumber() const
{
  return m_partLink.barcode();
}
