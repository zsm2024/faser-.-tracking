/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_ScintHit                  //
//                                                            //
//  Description: Handle for ScintHit's                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_SCINTHIT_H
#define SIMHITHANDLE_SCINTHIT_H

#include "VTI12TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

class ScintHit;
class SimHitHandle_ScintHit : public SimHitHandleBase {
public:

  SimHitHandle_ScintHit( const ScintHit * );
  virtual ~SimHitHandle_ScintHit();

  QString type() const { return "ScintHit"; };

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
