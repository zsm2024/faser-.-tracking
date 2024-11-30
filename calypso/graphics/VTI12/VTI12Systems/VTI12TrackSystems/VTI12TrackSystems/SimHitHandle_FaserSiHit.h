/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_SiHit                  //
//                                                            //
//  Description: Handle for SiHit's                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_FASERSIHIT_H
#define SIMHITHANDLE_FASERSIHIT_H

#include "VTI12TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

class FaserSiHit;
class SimHitHandle_FaserSiHit : public SimHitHandleBase {
public:

  SimHitHandle_FaserSiHit( const FaserSiHit * );
  virtual ~SimHitHandle_FaserSiHit();

  QString type() const { return "FaserSiHit"; };

  Amg::Vector3D momentumDirection() const;
  Amg::Vector3D posStart() const;
  Amg::Vector3D posEnd() const;
  double hitTime() const;

  const HepMcParticleLink& particleLink() const;

  Trk::TrackParameters * createTrackParameters() const;

private:

  class Imp;
  Imp * m_d;

};

#endif
