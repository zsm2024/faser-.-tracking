/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerSimEvent/FaserSiHit.h"
#include "TrackerSimEvent/FaserSiHitIdHelper.h"

// Default consdtructor needed by athenaroot
//
FaserSiHit::FaserSiHit( ) :
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
FaserSiHit::FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
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
FaserSiHit::FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
                     const HepGeom::Point3D<double> &localEndPosition,
                     const double energyLoss,
                     const double meanTime,
                     const int trackNumber,
                     const int station, 
                     const int plane,
                     const int row,
                     const int module,
                     const int sensor) :
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
  m_ID =  FaserSiHitIdHelper::GetHelper()->buildHitId(station, 
                                                      plane,
                                                      row,
                                                      module,
                                                      sensor);
}

// Constructor
FaserSiHit::FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
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
FaserSiHit::FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
                       const HepGeom::Point3D<double> &localEndPosition,
                       const double energyLoss,
                       const double meanTime,
                       const HepMcParticleLink &track,
                       const int station,
                       const int plane,
                       const int row,
                       const int module,
                       const int sensor) : 
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
  m_ID =  FaserSiHitIdHelper::GetHelper()->buildHitId(station,
                                                      plane,
                                                      row,
                                                      module,
                                                      sensor);
}

void FaserSiHit::ScaleLength(double sfactor) {
  m_stX *=  (float) sfactor;
  m_stY *=  (float) sfactor;
  m_stZ *=  (float) sfactor;
  m_enX *=  (float) sfactor;
  m_enY *=  (float) sfactor;
  m_enZ *=  (float) sfactor;
}

HepGeom::Point3D<double> FaserSiHit::localStartPosition() const
{
  return HepGeom::Point3D<double>((double) m_stX, (double) m_stY, (double) m_stZ);
}

HepGeom::Point3D<double> FaserSiHit::localEndPosition() const
{
  return HepGeom::Point3D<double>((double) m_enX, (double) m_enY, (double) m_enZ);
}

int FaserSiHit::getStation() const {
  return  FaserSiHitIdHelper::GetHelper()->getStation(m_ID);
}
int FaserSiHit::getPlane() const {
  return  FaserSiHitIdHelper::GetHelper()->getPlane(m_ID);
}
int FaserSiHit::getRow() const {
  return  FaserSiHitIdHelper::GetHelper()->getRow(m_ID);
}
int FaserSiHit::getModule() const {
  return  FaserSiHitIdHelper::GetHelper()->getModule(m_ID);
}
int FaserSiHit::getSensor() const {
  return  FaserSiHitIdHelper::GetHelper()->getSensor(m_ID);
}

void FaserSiHit::print() const {
  std::cout << "*** Faser Si Hit " << std::endl;
  std::cout << "          Station Number " << getStation() << std::endl;
  std::cout << "          Plane Number   " << getPlane() << std::endl;
  std::cout << "          Row Number     " << getRow() << std::endl;
  std::cout << "          Module Number  " << getModule() << std::endl;
  std::cout << "          Sensor         " << getSensor() << std::endl;
  std::cout << "          Energy (MeV)   " << energyLoss() << std::endl;
  if (particleLink().isValid())
  {
    std::cout << "          Barcode        " << particleLink().barcode() << std::endl;
#ifdef HEPMC3
    const HepMC3::ConstGenParticlePtr particle {particleLink()};
#else
    const HepMC::GenParticle* particle = (particleLink());
#endif
    std::cout << "          PDG ID         " << particle->pdg_id() << std::endl;
  }
  else
  {
    std::cout << "Barcode/PDG ID Unknown   " << std::endl;
  }
}

int FaserSiHit::trackNumber() const
{
  return m_partLink.barcode();
}
