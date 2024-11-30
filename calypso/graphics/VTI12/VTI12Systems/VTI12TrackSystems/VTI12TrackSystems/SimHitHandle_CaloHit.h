/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class SimHitHandle_CaloHit                  //
//                                                            //
//  Description: Handle for CaloHit's                           //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef SIMHITHANDLE_CALOHIT_H
#define SIMHITHANDLE_CALOHIT_H

#include "VTI12TrackSystems/SimHitHandleBase.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "TrkParameters/TrackParameters.h"

class CaloHit;
class SimHitHandle_CaloHit : public SimHitHandleBase {
public:

  SimHitHandle_CaloHit( const CaloHit * );
  virtual ~SimHitHandle_CaloHit();

  QString type() const { return "CaloHit"; };

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
