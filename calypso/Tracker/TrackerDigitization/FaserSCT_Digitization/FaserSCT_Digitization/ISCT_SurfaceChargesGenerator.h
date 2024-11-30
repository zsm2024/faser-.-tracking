/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * ISCT_SurfaceChargesGenerator.h
 * Header file for abstract base class ISCT_SurfaceChargesGenerator
 * (c) ATLAS Detector software
 * Interface for the Sct_SurfaceCharge generator classes
 * Version  23/08/2007 Kondo Gnanvo
 */

#ifndef FASERSCT_DIGITIZATION_ISCT_SURFACECHARGESGENERATOR_H
#define FASERSCT_DIGITIZATION_ISCT_SURFACECHARGESGENERATOR_H

// Input/output classes
#include <list>
#include "FaserSiDigitization/SiSurfaceCharge.h"
#include "HitManagement/TimedHitPtr.h"

#include "GaudiKernel/IAlgTool.h"

class FaserSiHit;
class EventContext;

namespace TrackerDD {
  class SiDetectorElement;
}
namespace CLHEP {
  class HepRandomEngine;
}

class ISiSurfaceChargesInserter
{
 public:
  virtual ~ISiSurfaceChargesInserter() {}
  virtual void operator() (const SiSurfaceCharge& scharge) const = 0;
};

static const InterfaceID IID_ISCT_SurfaceChargesGenerator("ISCT_SurfaceChargesGenerator",1,0);

class ISCT_SurfaceChargesGenerator : virtual public IAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:

  //Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ISCT_SurfaceChargesGenerator; }

  // Destructor:
  virtual ~ISCT_SurfaceChargesGenerator() {}

  virtual void process(const TrackerDD::SiDetectorElement* ele,
                       const TimedHitPtr<FaserSiHit>& phit,
                       const ISiSurfaceChargesInserter& inserter, 
                       CLHEP::HepRandomEngine * rndmEngine,
                       const EventContext& ctx) const =0;
  virtual void setFixedTime(float fixedTime) =0;
};

#endif // FASERSCT_DIGITIZATION_ISCT_SURFACECHARGESGENERATOR_H
