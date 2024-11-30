/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1JobConfigInfo                  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12Utils/VP1JobConfigInfo.h"
#include "VTI12Utils/VP1DetInfo.h"
#include "VTI12Utils/VP1SGAccessHelper.h"
#include "VTI12Utils/VP1SGContentsHelper.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/VP1AthenaPtrs.h"

#include "GeoModelKernel/GeoVolumeCursor.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"
#include "StoreGate/DataHandle.h"

#include <QString>

//____________________________________________________________________
class VP1JobConfigInfo::Imp {
public:
  static void ensureInit();
  static bool actualInit(StoreGateSvc* detStore);
  static bool initialised;

  static GeoPVConstLink geoModelWorld;
  static bool hasGeoModelExperiment;
  static bool hasEmulsionGeometry;
  static bool hasVetoGeometry;
  static bool hasVetoNuGeometry;
  static bool hasTriggerGeometry;
  static bool hasPreshowerGeometry;
  static bool hasSCTGeometry;
  static bool hasEcalGeometry;
  static bool hasCavernInfraGeometry;

  static void turnOffAll();
};

GeoPVConstLink VP1JobConfigInfo::Imp::geoModelWorld;
// init default values
bool VP1JobConfigInfo::Imp::initialised = false;
bool VP1JobConfigInfo::Imp::hasGeoModelExperiment = false;
bool VP1JobConfigInfo::Imp::hasEmulsionGeometry = false;
bool VP1JobConfigInfo::Imp::hasVetoGeometry = false;
bool VP1JobConfigInfo::Imp::hasVetoNuGeometry = false;
bool VP1JobConfigInfo::Imp::hasTriggerGeometry = false;
bool VP1JobConfigInfo::Imp::hasPreshowerGeometry = false;
bool VP1JobConfigInfo::Imp::hasSCTGeometry = false;
bool VP1JobConfigInfo::Imp::hasEcalGeometry = false;
bool VP1JobConfigInfo::Imp::hasCavernInfraGeometry = false;

//____________________________________________________________________
void VP1JobConfigInfo::Imp::turnOffAll()
{
  hasGeoModelExperiment = false;
  hasEmulsionGeometry = false;
  hasVetoGeometry = false;
  hasVetoNuGeometry = false;
  hasTriggerGeometry = false;
  hasPreshowerGeometry = false;
  hasSCTGeometry = false;
  hasEcalGeometry = false;
  hasCavernInfraGeometry = false;
}

//____________________________________________________________________
void VP1JobConfigInfo::Imp::ensureInit()
{
  if (initialised)
    return;
  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VTI12JobConfigInfo initialising");
  initialised = true;
  if (!actualInit(VP1AthenaPtrs::detectorStore())) {
    VP1Msg::message("VTI12JobConfigInfo ERROR: Problems initialising. Detector store. "
		    "Will assume all subsystems are off in this job!");
    turnOffAll();
  }
  if (VP1Msg::verbose()) {
    VP1Msg::messageVerbose("VTI12JobConfigInfo => Found job configuration:");
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasGeoModelExperiment = "+QString(hasGeoModelExperiment?"On":"Off"));
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasEmulsionGeometry = "+QString(hasEmulsionGeometry?"On":"Off"));
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasVetoGeometry = "+QString(hasVetoGeometry?"On":"Off"));
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasVetoNuGeometry = "+QString(hasVetoNuGeometry?"On":"Off"));
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasTriggerGeometry = "+QString(hasTriggerGeometry?"On":"Off"));
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasPreshowerGeometry = "+QString(hasPreshowerGeometry?"On":"Off"));

    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasSCTGeometry = "+QString(hasSCTGeometry?"On":"Off"));

    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasEcalGeometry = "+QString(hasEcalGeometry?"On":"Off"));
    VP1Msg::messageVerbose("VTI12JobConfigInfo => hasCavernInfraGeometry = "+QString(hasCavernInfraGeometry?"On":"Off"));

  }

}

//____________________________________________________________________
bool VP1JobConfigInfo::hasGeoModelExperiment() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasGeoModelExperiment; }
bool VP1JobConfigInfo::hasEmulsionGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasEmulsionGeometry; }
bool VP1JobConfigInfo::hasVetoGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasVetoGeometry; }
bool VP1JobConfigInfo::hasVetoNuGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasVetoNuGeometry; }
bool VP1JobConfigInfo::hasTriggerGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasTriggerGeometry; }
bool VP1JobConfigInfo::hasPreshowerGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasPreshowerGeometry; }

bool VP1JobConfigInfo::hasSCTGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasSCTGeometry; }

bool VP1JobConfigInfo::hasEcalGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasEcalGeometry; }
bool VP1JobConfigInfo::hasCavernInfraGeometry() { if (!Imp::initialised) Imp::ensureInit(); return Imp::hasCavernInfraGeometry; }

//____________________________________________________________________
bool VP1JobConfigInfo::Imp::actualInit( StoreGateSvc* detStore )
{
  if (!detStore)
    return false;
  
  VP1SGContentsHelper sg_contents(detStore);
  const QString geomodelkey = "FASER";  // Because it was too hard to make this configurable like the 2151516 other useless options...
  if (!sg_contents.contains<GeoModelExperiment>(geomodelkey)) {
    if (VP1Msg::verbose())
      VP1Msg::messageVerbose("VTI12JobConfigInfo: No GeoModelExperiment in detectorStore. Concluding all subsystems are off.");
    turnOffAll();
    return true;
  }

  VP1SGAccessHelper sg_access(detStore);

  const GeoModelExperiment * theExpt;
  if (!sg_access.retrieve(theExpt,geomodelkey)) {
    if (VP1Msg::verbose())
      VP1Msg::messageVerbose("VTI12JobConfigInfo: No GeoModelExperiment in detectorStore. Concluding all subsystems are off.");
    turnOffAll();
    return true;
  }
  if (VP1Msg::verbose())
    VP1Msg::messageVerbose("VTI12JobConfigInfo: Retrieved GeoModelExperiment/"+geomodelkey);

  const GeoPhysVol * worldPhysVol = theExpt->getPhysVol();
  if (!worldPhysVol) {
    VP1Msg::message("VTI12JobConfigInfo: ERROR: GeoModelExperiment has null physical volume.");
    turnOffAll();
    return false;
  }
  if (VP1Msg::verbose())
      VP1Msg::messageVerbose("VTI12JobConfigInfo: Retrieved GeoModelExperiment physical volumes");

  hasGeoModelExperiment = true;
  PVConstLink world(worldPhysVol);
  VP1Msg::messageDebug("VTI12JobConfigInfo: called world(worldPhysVol)");

  GeoVolumeCursor av(world);


  while (!av.atEnd()) {
    std::string name = av.getName();
    VP1Msg::message( QString { name.c_str() } );
    if ( !hasEmulsionGeometry && name=="Emulsion") hasEmulsionGeometry = true;
    if ( !hasVetoGeometry && name=="Veto") hasVetoGeometry = true;
    if ( !hasVetoNuGeometry && name=="VetoNu") hasVetoNuGeometry = true;
    if ( !hasTriggerGeometry && name=="Trigger") hasTriggerGeometry = true;
    if ( !hasPreshowerGeometry && name=="Preshower") hasPreshowerGeometry = true;
    if ( !hasSCTGeometry && name=="SCT") hasSCTGeometry = true;
    if ( !hasEcalGeometry && name=="Ecal") hasEcalGeometry = true;
    if ( !hasCavernInfraGeometry && name == "Trench") hasCavernInfraGeometry = true;

    //FIXME: Look for CavernInfra as well!!!

    av.next(); // increment volume cursor.
  }

  Imp::geoModelWorld = worldPhysVol;

  return true;
}

//____________________________________________________________________
const GeoPVConstLink * VP1JobConfigInfo::geoModelWorld()
{
  Imp::ensureInit();
  return Imp::geoModelWorld == GeoPVConstLink() ? 0 : &Imp::geoModelWorld;
}
