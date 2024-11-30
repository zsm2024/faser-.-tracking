/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_NeutrinoHit                  //
//                                                            //
//  Description: Handle for NeutrinoHit's                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_NEUTRINOHIT_H
#define SIMHITHANDLE_NEUTRINOHIT_H

#include "VTI12TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

class NeutrinoHit;
class SimHitHandle_NeutrinoHit : public SimHitHandleBase {
public:

  SimHitHandle_NeutrinoHit( const NeutrinoHit * );
  virtual ~SimHitHandle_NeutrinoHit();

  QString type() const { return "NeutrinoHit"; };

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
