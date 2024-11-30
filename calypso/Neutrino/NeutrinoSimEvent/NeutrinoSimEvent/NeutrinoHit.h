/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoHit.h
//   Header file for class NeutrinoHit
///////////////////////////////////////////////////////////////////
// Class for Emulsion hits
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOSIMEVENT_NEUTRINOHIT_H
#define NEUTRINOSIMEVENT_NEUTRINOHIT_H

// Data members classes
#include "CLHEP/Geometry/Point3D.h"
#include "GeneratorObjects/HepMcParticleLink.h"

class NeutrinoHit  {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   local start position of the energy deposit
  //   local end position of the energy deposit
  //   deposited energy
  //   time of energy deposition
  //   number of track which released this energy
  //
  NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const int trackNumber,
           const unsigned int id);

  NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const int trackNumber,
           const int, const int, const int);
  // Constructor with parameters:
  //   local start position of the energy deposit
  //   local end position of the energy deposit
  //   deposited energy
  //   time of energy deposition
  //   link to particle which released this energy
  //
  NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const HepMcParticleLink &track,
           const unsigned int id);

  NeutrinoHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const HepMcParticleLink &track,
           const int, const int, const int);
  // needed by athenaRoot
  NeutrinoHit();

  // Destructor:
  virtual ~NeutrinoHit() = default; 
  
  //move assignment defaulted
  NeutrinoHit & operator = (NeutrinoHit &&) = default;
  //assignment defaulted
  NeutrinoHit & operator = (const NeutrinoHit &) = default;
  //copy c'tor defaulted
  NeutrinoHit(const NeutrinoHit &) = default;
  //move c'tor defaulted
  NeutrinoHit(NeutrinoHit &&) noexcept = default;

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  unsigned int identify() const;

  // local start position of the energy deposit:
  HepGeom::Point3D<double> localStartPosition() const;

  HepGeom::Point3D<double> localEndPosition() const;

  // deposited energy:
  double energyLoss() const;

  // time of energy deposition: FIXME name!
  double meanTime() const;

  // Set the time of energy deposition: FIXME name!
  void setMeanTime(float meanTime);

  // number of track which released this energy:
  int trackNumber() const;

  // link to the particle generating the hit
  const HepMcParticleLink& particleLink() const;

  // Film
  int getFilm() const;

  // Base
  int getBase() const;

  // Module
  int getModule() const;

  // some print-out:
  void print() const;

  bool operator < (const NeutrinoHit& rhs) const
  {return m_ID < rhs.m_ID;}

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////
  // Scale the length, used to go from cm to mm, of whatever we like.
  void ScaleLength(double);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:

  float m_stX, m_stY, m_stZ;
  float m_enX, m_enY, m_enZ;
  float m_energyLoss; // deposited energy
  float m_meanTime; // time of energy deposition
  HepMcParticleLink m_partLink;
  unsigned int m_ID;
public:
  // enum
  //   { xDep = 2, xPhi = 0, xEta = 1};
};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline unsigned int NeutrinoHit::identify() const
{
  return m_ID;
}

inline double NeutrinoHit::energyLoss() const
{
  return (double) m_energyLoss;
}

inline double NeutrinoHit::meanTime() const
{
  return (double) m_meanTime;
}

inline void NeutrinoHit::setMeanTime(float meanTime)
{
  m_meanTime=meanTime;
}

inline const HepMcParticleLink& NeutrinoHit::particleLink() const
{
  return m_partLink;
}



///////////////////////////////////////////////////////////////////
// open functions:
///////////////////////////////////////////////////////////////////

inline float hitTime(const NeutrinoHit& hit)
{
  return (float) hit.meanTime();
}

#endif // NEUTRINOSIMEVENT_NEUTRINOHIT_H
