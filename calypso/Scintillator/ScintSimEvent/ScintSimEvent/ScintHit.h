/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintHit.h
//   Header file for class ScintHit
///////////////////////////////////////////////////////////////////
// Class for the Veto, Trigger and Preshower hits
///////////////////////////////////////////////////////////////////

#ifndef SCINTSIMEVENT_SCINTHIT_H
#define SCINTSIMEVENT_SCINTHIT_H

// Data members classes
#include "CLHEP/Geometry/Point3D.h"
#include "GeneratorObjects/HepMcParticleLink.h"

class Identifier;

class ScintHit  {

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
  ScintHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const int trackNumber,
           const unsigned int id);

  ScintHit(const HepGeom::Point3D<double> &localStartPosition,
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
  ScintHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const HepMcParticleLink &track,
           const unsigned int id);

  ScintHit(const HepGeom::Point3D<double> &localStartPosition,
           const HepGeom::Point3D<double> &localEndPosition,
           const double energyLoss,
           const double meanTime,
           const HepMcParticleLink &track,
           const int, const int, const int);
  // needed by athenaRoot
  ScintHit();

  // Destructor:
  virtual ~ScintHit() = default; 
  
  //move assignment defaulted
  ScintHit & operator = (ScintHit &&) = default;
  //assignment defaulted
  ScintHit & operator = (const ScintHit &) = default;
  //copy c'tor defaulted
  ScintHit(const ScintHit &) = default;
  //move c'tor defaulted
  ScintHit(ScintHit &&) noexcept = default;

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // This is the HitId, used in Geant. This is not the detector Identifier
  unsigned int identify() const;

  // This is the detector readout Identifier (pmt Identifier)
  // provided by ScintHitIdHelper::getIdentifier
  Identifier getIdentifier() const;

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

  // Veto, trigger or preshower?
  bool isVeto() const;
  bool isTrigger() const;
  bool isPreshower() const;
  bool isVetoNu() const;

  // Station
  int getStation() const;

  // Plate
  int getPlate() const;

  // some print-out:
  void print() const;

  bool operator < (const ScintHit& rhs) const
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

inline unsigned int ScintHit::identify() const
{
  return m_ID;
}

inline double ScintHit::energyLoss() const
{
  return (double) m_energyLoss;
}

inline double ScintHit::meanTime() const
{
  return (double) m_meanTime;
}

inline void ScintHit::setMeanTime(float meanTime)
{
  m_meanTime=meanTime;
}

inline const HepMcParticleLink& ScintHit::particleLink() const
{
  return m_partLink;
}



///////////////////////////////////////////////////////////////////
// open functions:
///////////////////////////////////////////////////////////////////

inline float hitTime(const ScintHit& hit)
{
  return (float) hit.meanTime();
}

#endif // SCINTSIMEVENT_SCINTHIT_H
