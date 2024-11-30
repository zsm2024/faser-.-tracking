/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutrinoSimEvent/NeutrinoHit.h"
#include "NeutrinoSimEvent/NeutrinoHitIdHelper.h"

// Default consdtructor needed by athenaroot
//
NeutrinoHit::NeutrinoHit( ) :
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
NeutrinoHit::NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
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
NeutrinoHit::NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const int trackNumber,
                  //  const int veto_trigger_preshower, 
                   const int module, 
                   const int base,
                   const int film) :
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
  m_ID =  NeutrinoHitIdHelper::GetHelper()->buildHitId( module, base, film);
}

// Constructor
NeutrinoHit::NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
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
NeutrinoHit::NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
                   const HepGeom::Point3D<double> &localEndPosition,
                   const double energyLoss,
                   const double meanTime,
                   const HepMcParticleLink &track,
                  //  const int veto_trigger_preshower, 
                   const int module,
                   const int base,
                   const int film) : 
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
  m_ID =  NeutrinoHitIdHelper::GetHelper()->buildHitId( module, base, film);
}

void NeutrinoHit::ScaleLength(double sfactor) {
  m_stX *=  (float) sfactor;
  m_stY *=  (float) sfactor;
  m_stZ *=  (float) sfactor;
  m_enX *=  (float) sfactor;
  m_enY *=  (float) sfactor;
  m_enZ *=  (float) sfactor;
}

HepGeom::Point3D<double> NeutrinoHit::localStartPosition() const
{
  return HepGeom::Point3D<double>((double) m_stX, (double) m_stY, (double) m_stZ);
}

HepGeom::Point3D<double> NeutrinoHit::localEndPosition() const
{
  return HepGeom::Point3D<double>((double) m_enX, (double) m_enY, (double) m_enZ);
}

int NeutrinoHit::getFilm() const {
  return  NeutrinoHitIdHelper::GetHelper()->getFilm(m_ID);
}

int NeutrinoHit::getBase() const {
  return  NeutrinoHitIdHelper::GetHelper()->getBase(m_ID);
}

int NeutrinoHit::getModule() const {
  return  NeutrinoHitIdHelper::GetHelper()->getModule(m_ID);
}

void NeutrinoHit::print() const {
  std::cout << "*** Neutrino Hit" << std::endl;
  std::cout << "          Module Number " << getModule() << std::endl;
  std::cout << "          Base Number   "   << getBase() << std::endl;
  std::cout << "          Film Number   " << getFilm() << std::endl;
  std::cout << "          Energy (MeV)  " << energyLoss() << std::endl;
  if (particleLink().isValid())
  {
    std::cout << "          Barcode       " << particleLink().barcode() << std::endl;
#ifdef HEPMC3
    const HepMC3::ConstGenParticlePtr particle {particleLink()};
#else
    const HepMC::GenParticle* particle = (particleLink());
#endif
    std::cout << "          PDG ID        " << particle->pdg_id() << std::endl;
  }
  else
  {
    std::cout << "Barcode/PDG ID Unknown  " << std::endl;
  }
}

int NeutrinoHit::trackNumber() const
{
  return m_partLink.barcode();
}
