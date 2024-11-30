/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class VP1DetInfo                          //
//                                                            //
//  Description: Convenient and safe access to detector       //
//               managers and identifier decoding             //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VTI12UTILS_VP1DETINFO_H
#define VTI12UTILS_VP1DETINFO_H

class IVP1System;
class StoreGateSvc;
namespace NeutrinoDD { class EmulsionDetectorManager; }
namespace TrackerDD { class SCT_DetectorManager; }
namespace ScintDD { class VetoDetectorManager; }
namespace ScintDD { class VetoNuDetectorManager; }
namespace ScintDD { class TriggerDetectorManager; }
namespace ScintDD { class PreshowerDetectorManager; }
namespace CaloDD  { class EcalDetectorManager; }

class FaserDetectorID;
class EmulsionDetectorID;
class ScintDetectorID;
class VetoID;
class VetoNuID;
class TriggerID;
class PreshowerID;
class FaserSCT_ID;
class EcalID;
class EmulsionID;

class Identifier;

class VP1DetInfo {
public:
  static void ensureInit( IVP1System* ) {}
  static void ensureInit( StoreGateSvc* ) {}

  //NB: Any of the methods returning pointers in this class might
  //return null. And it is never allowed to delete any of the returned
  //pointers!

  static const NeutrinoDD::EmulsionDetectorManager * emulsionDetMgr();

  static const ScintDD::VetoDetectorManager * vetoDetMgr();
  static const ScintDD::VetoNuDetectorManager * vetoNuDetMgr();
  static const ScintDD::TriggerDetectorManager * triggerDetMgr();
  static const ScintDD::PreshowerDetectorManager * preshowerDetMgr();

  static const TrackerDD::SCT_DetectorManager * sctDetMgr();

  static const CaloDD::EcalDetectorManager * ecalDetMgr();

  //Base identifier helper:
  static const FaserDetectorID * faserIDHelper();

  //Common specialised identifier helpers:

   static const EmulsionID * emulsionIDHelper();

   static const VetoID * vetoIDHelper();
   static const VetoNuID * vetoNuIDHelper();
   static const TriggerID * triggerIDHelper();
   static const PreshowerID * preshowerIDHelper();

  static const FaserSCT_ID * sctIDHelper();

  static const EcalID * ecalIDHelper();

  //Todo: Add more as needed.

  //Combines use of the atlasIDHelper and VP1JobConfigInfo to spot
  //unsafe identifiers (i.e. a pixel identifier is unsafe if the.pixel
  //geometry is not lodaded):
  static bool isUnsafe(const Identifier&);

private:
  VP1DetInfo(){}
  ~VP1DetInfo(){}
  class Imp;
};


#endif
