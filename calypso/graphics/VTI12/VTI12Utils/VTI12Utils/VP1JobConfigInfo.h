/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1JobConfigInfo                    //
//                                                            //
//  Description: Provides convenient and safe information     //
//               about the configuration of a given job       //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VTI12UTILS_VP1JOBCONFIGINFO_H
#define VTI12UTILS_VP1JOBCONFIGINFO_H

// GeoModel
#include "GeoPrimitives/GeoPrimitives.h"
//
#include "GeoModelKernel/GeoVPhysVol.h"
class IVP1System;
class StoreGateSvc;

class VP1JobConfigInfo {
public:

  static void ensureInit( IVP1System* ) {}//obsolete
  static void ensureInit( StoreGateSvc* ) {}//obsolete

  //These next methods tells us what geomodel parts are initialised:
  static bool hasGeoModelExperiment();//If GeoModelExperiment/"FASER" can be retrieved

  static bool hasEmulsionGeometry();

  static bool hasVetoGeometry();
  static bool hasVetoNuGeometry();
  static bool hasTriggerGeometry();
  static bool hasPreshowerGeometry();

  static bool hasSCTGeometry();
  
  static bool hasEcalGeometry();

  static bool hasCavernInfraGeometry();

  //Top geomodel volume link (not strictly "JobConfig", but here it is):
  static const GeoPVConstLink * geoModelWorld();//might return 0

private:
  VP1JobConfigInfo(){}
  ~VP1JobConfigInfo(){}
  class Imp;
};

#endif
