/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOFASERCALOHIT_h
#define GEOADAPTORS_GEOFASERCALOHIT_h
//----------------------------------------------------------//
//                                                          //
// An adaptor for CaloHits.                                 //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows ScintHits to behave            //
// as if they knew which detector they were in.             //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class CaloHit;
class EcalID;
namespace CaloDD {
  class EcalDetectorManager;
}

class GeoFaserCaloHit {

 public:

  // Constructor:
  GeoFaserCaloHit(const CaloHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Underlying hit.
  const CaloHit &data() const { return *m_hit;}

  // Is this hit ok?

  operator bool () const { return s_ecal; }

 private:
  
  static void init();

  const CaloHit                                   *m_hit;
  static const CaloDD::EcalDetectorManager        *s_ecal;
  static const EcalID                             *s_eID;
};

#include "FaserGeoAdaptors/GeoFaserCaloHit.icc"

#endif
