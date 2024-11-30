/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOADAPTORS_GEONEUTRINOHIT_h
#define GEOADAPTORS_GEONEUTRINOHIT_h
//----------------------------------------------------------//
//                                                          //
// An adaptor for NeutrinoHits.                                //
//                                                          //
// Joe Boudreau Feb 04.                                     //
//                                                          //
// This adaptor class allows NeutrinoHits to behave            //
// as if they knew which detector they were in.             //
//                                                          //
//                                                          //
//----------------------------------------------------------//
#include "CLHEP/Geometry/Point3D.h"
class NeutrinoHit;
class EmulsionID;
namespace NeutrinoDD {
  class EmulsionDetectorManager;
}

class GeoNeutrinoHit {

 public:

  // Constructor:
  GeoNeutrinoHit(const NeutrinoHit & h);

  // Get the absolute global position:
  HepGeom::Point3D<double> getGlobalPosition() const;

  // Underlying hit.
  const NeutrinoHit &data() const { return *m_hit;}

  // Is this hit ok?

  operator bool () const { return s_emulsion; }

 private:
  
  static void init();

  const NeutrinoHit                                 *m_hit;
  static const NeutrinoDD::EmulsionDetectorManager  *s_emulsion;
  static const EmulsionID                           *s_nID;
};

#include "FaserGeoAdaptors/GeoNeutrinoHit.icc"

#endif
