/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEOFASERSIHIT_h
#define GEOADAPTORS_GEOFASERSIHIT_h
//----------------------------------------------------------//
//                                                          //
// An adaptor for SiHits.                                   //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows SiHits to behave               //
// as if they knew which detector they were in.             //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class FaserSiHit;
class FaserSCT_ID;
namespace TrackerDD {
  class SCT_DetectorManager;
}

class GeoFaserSiHit {

 public:

  // Constructor:
  GeoFaserSiHit(const FaserSiHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Underlying hit.
  const FaserSiHit &data() const { return *m_hit;}

  // Is this hit ok?

  operator bool () const { return s_sct;  }

 private:
  
  static void init();

  const FaserSiHit                                *m_hit;
  static const TrackerDD::SCT_DetectorManager     *s_sct;
  static const FaserSCT_ID                        *s_sID;
};

#include "FaserGeoAdaptors/GeoFaserSiHit.icc"

#endif
