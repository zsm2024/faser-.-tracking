/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSiHit.h
//   Header file for class FaserSiHit
///////////////////////////////////////////////////////////////////
// Class for the Faser SCT hits
///////////////////////////////////////////////////////////////////

#ifndef TRACKERSIMEVENT_FASERSIHIT_H
#define TRACKERSIMEVENT_FASERSIHIT_H

// Data members classes
#include "CLHEP/Geometry/Point3D.h"
#include "GeneratorObjects/HepMcParticleLink.h"

class FaserSiHit  {

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
  FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const int trackNumber,
             const unsigned int id);

  FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const int trackNumber,
             const int, const int, const int, const int, const int);
  // Constructor with parameters:
  //   local start position of the energy deposit
  //   local end position of the energy deposit
  //   deposited energy
  //   time of energy deposition
  //   link to particle which released this energy
  //
  FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const HepMcParticleLink &track,
             const unsigned int id);

  FaserSiHit(const HepGeom::Point3D<double> &localStartPosition,
             const HepGeom::Point3D<double> &localEndPosition,
             const double energyLoss,
             const double meanTime,
             const HepMcParticleLink &track,
             const int, const int, const int, const int, const int);
  // needed by athenaRoot
  FaserSiHit();

  // Destructor:
  virtual ~FaserSiHit() = default; 
  
  //move assignment defaulted
  FaserSiHit & operator = (FaserSiHit &&) = default;
  //assignment defaulted
  FaserSiHit & operator = (const FaserSiHit &) = default;
  //copy c'tor defaulted
  FaserSiHit(const FaserSiHit &) = default;
  //move c'tor defaulted
  FaserSiHit(FaserSiHit &&) noexcept = default;

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

  // Station
  int getStation() const;

  // Plane
  int getPlane() const;

  // Row
  int getRow() const;

  // Module
  int getModule() const;

  // Sensor
  int getSensor() const;

  // some print-out:
  void print() const;

  bool operator < (const FaserSiHit& rhs) const
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
  enum
    { xDep = 0, xPhi = 1, xEta = 2};
};


///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

inline unsigned int FaserSiHit::identify() const
{
  return m_ID;
}

inline double FaserSiHit::energyLoss() const
{
  return (double) m_energyLoss;
}

inline double FaserSiHit::meanTime() const
{
  return (double) m_meanTime;
}

inline void FaserSiHit::setMeanTime(float meanTime)
{
  m_meanTime=meanTime;
}

inline const HepMcParticleLink& FaserSiHit::particleLink() const
{
  return m_partLink;
}



///////////////////////////////////////////////////////////////////
// open functions:
///////////////////////////////////////////////////////////////////

inline float hitTime(const FaserSiHit& hit)
{
  return (float) hit.meanTime();
}

#endif // TRACKERSIMEVENT_FASERSIHIT_H
