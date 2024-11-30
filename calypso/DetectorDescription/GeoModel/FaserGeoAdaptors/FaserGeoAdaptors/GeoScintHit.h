/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOSCINTHIT_h
#define GEOADAPTORS_GEOSCINTHIT_h
//----------------------------------------------------------//
//                                                          //
// An adaptor for ScintHits.                                //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows ScintHits to behave            //
// as if they knew which detector they were in.             //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class ScintHit;
class VetoID;
class TriggerID;
class PreshowerID;
class VetoNuID;
namespace ScintDD {
  class VetoDetectorManager;
  class TriggerDetectorManager;
  class PreshowerDetectorManager;
  class VetoNuDetectorManager;
}

class GeoScintHit {

 public:

  // Constructor:
  GeoScintHit(const ScintHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Underlying hit.
  const ScintHit &data() const { return *m_hit;}

  // Is this hit ok?

  operator bool () const { return s_veto || s_trigger || s_preshower || s_vetonu; }

 private:
  
  static void init();

  const ScintHit                                  *m_hit;
  static const ScintDD::VetoDetectorManager       *s_veto;
  static const ScintDD::VetoNuDetectorManager     *s_vetonu;
  static const ScintDD::TriggerDetectorManager    *s_trigger;
  static const ScintDD::PreshowerDetectorManager  *s_preshower;
  static const VetoID                             *s_vID;
  static const VetoNuID                           *s_vnID;
  static const TriggerID                          *s_tID;
  static const PreshowerID                        *s_pID;
};

#include "FaserGeoAdaptors/GeoScintHit.icc"

#endif
