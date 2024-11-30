/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class VP1DetInfo                        //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: March 2008                               //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12Utils/VP1DetInfo.h"
#include "VTI12Utils/VP1JobConfigInfo.h"
#include "VTI12Utils/VP1SGAccessHelper.h"
#include "VTI12Utils/VP1SGContentsHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/IVP1System.h"
#include "VP1Base/VP1AthenaPtrs.h"

#include "NeutrinoReadoutGeometry/EmulsionDetectorManager.h"

#include "ScintReadoutGeometry/VetoDetectorManager.h"
#include "ScintReadoutGeometry/VetoNuDetectorManager.h"
#include "ScintReadoutGeometry/TriggerDetectorManager.h"
#include "ScintReadoutGeometry/PreshowerDetectorManager.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

#include "CaloReadoutGeometry/EcalDetectorManager.h"

#include "FaserDetDescr/FaserDetectorID.h"

#include "NeutrinoIdentifier/EmulsionID.h"

#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"

#include "TrackerIdentifier/FaserSCT_ID.h"

#include "FaserCaloIdentifier/EcalID.h"

#include "Identifier/Identifier.h"

//____________________________________________________________________
class VP1DetInfo::Imp {
public:
  template <class T>
  static const T * cachedRetrieve(const T*& cachedPtr, const char* key, const bool& configallows );

  static bool m_initialised;
  static const char m_badInitFlag;//Address of this means bad initialisation.

  static const NeutrinoDD::EmulsionDetectorManager * m_emulsionDetMgr;

  static const ScintDD::VetoDetectorManager * m_vetoDetMgr;
  static const ScintDD::VetoNuDetectorManager * m_vetoNuDetMgr;
  static const ScintDD::TriggerDetectorManager * m_triggerDetMgr;
  static const ScintDD::PreshowerDetectorManager * m_preshowerDetMgr;

  static const TrackerDD::SCT_DetectorManager * m_sctDetMgr;

  static const CaloDD::EcalDetectorManager * m_ecalDetMgr;

  static const FaserDetectorID * m_faserIDHelper;

  static const EmulsionID *   m_emulsionIDHelper;

  static const VetoID *       m_vetoIDHelper;
  static const VetoNuID *     m_vetoNuIDHelper;
  static const TriggerID *    m_triggerIDHelper;
  static const PreshowerID *  m_preshowerIDHelper;

  static const FaserSCT_ID * m_sctIDHelper;

  static const EcalID *      m_ecalIDHelper;
};

bool VP1DetInfo::Imp::m_initialised = false;
const char VP1DetInfo::Imp::m_badInitFlag = ' ';

const NeutrinoDD::EmulsionDetectorManager * VP1DetInfo::Imp::m_emulsionDetMgr = 0;

const ScintDD::VetoDetectorManager * VP1DetInfo::Imp::m_vetoDetMgr = 0;
const ScintDD::VetoNuDetectorManager * VP1DetInfo::Imp::m_vetoNuDetMgr = 0;
const ScintDD::TriggerDetectorManager * VP1DetInfo::Imp::m_triggerDetMgr = 0;
const ScintDD::PreshowerDetectorManager * VP1DetInfo::Imp::m_preshowerDetMgr = 0;

const TrackerDD::SCT_DetectorManager * VP1DetInfo::Imp::m_sctDetMgr = 0;

const CaloDD::EcalDetectorManager * VP1DetInfo::Imp::m_ecalDetMgr = 0;

const FaserDetectorID * VP1DetInfo::Imp::m_faserIDHelper = 0;

const EmulsionID * VP1DetInfo::Imp::m_emulsionIDHelper = 0;

const VetoID * VP1DetInfo::Imp::m_vetoIDHelper = 0;
const VetoNuID * VP1DetInfo::Imp::m_vetoNuIDHelper = 0;
const TriggerID * VP1DetInfo::Imp::m_triggerIDHelper = 0;
const PreshowerID * VP1DetInfo::Imp::m_preshowerIDHelper = 0;

const FaserSCT_ID * VP1DetInfo::Imp::m_sctIDHelper = 0;

const EcalID * VP1DetInfo::Imp::m_ecalIDHelper = 0;

//____________________________________________________________________
template <class T>
const T * VP1DetInfo::Imp::cachedRetrieve(const T*& cachedPtr, const char* preferredKey, const bool& configallows ) {
  const T * bad = static_cast<const T*>(static_cast<const void*>(&m_badInitFlag));
  if (cachedPtr)
    return ( cachedPtr==bad? 0 : cachedPtr );
  QString key(preferredKey);
  if (!configallows) {
    VP1Msg::messageDebug("VTI12DetInfo WARNING: Will not attempt to get (type="+QString(typeid(T).name())+", key="+key+") due to missing/disabled features in job!");
    cachedPtr = bad;
    return 0;
  }
  if (!VP1SGContentsHelper(VP1AthenaPtrs::detectorStore()).contains<T>(key)) {
    //Try to gracefully guess at a different key:
    QStringList keys = VP1SGContentsHelper(VP1AthenaPtrs::detectorStore()).getKeys<T>();
    if (keys.empty()) {
      VP1Msg::messageDebug("VTI12DetInfo WARNING: Could not find (type="+QString(typeid(T).name())+") in detector store (expected key="+key+")");
      cachedPtr = bad;
      return 0;
    }
    if (keys.count()>1) {
      VP1Msg::messageDebug("VTI12DetInfo WARNING: Could not find (type="+QString(typeid(T).name())+", key="+key+") in detector store, and could not uniquely guess at alternate key.");
      cachedPtr = bad;
      return 0;
    }
    VP1Msg::messageDebug("VTI12DetInfo WARNING: Could not find (type="+QString(typeid(T).name())+", key="+key+") in detector store. Trying with key="+keys.first()+")");
    key = keys.first();
  }
  if (!VP1SGAccessHelper(VP1AthenaPtrs::detectorStore()).retrieve(cachedPtr,key)||!cachedPtr) {
    VP1Msg::messageDebug("VTI12DetInfo WARNING: Could not retrieve (type="+QString(typeid(T).name())+", key="+key+") from detector store!");
    cachedPtr = bad;
    return 0;
  }
  VP1Msg::messageVerbose("VTI12DetInfo Succesfully retrieved (type="+QString(typeid(T).name())+", key="+key+") from detector store!");
  return cachedPtr;

}

const NeutrinoDD::EmulsionDetectorManager * VP1DetInfo::emulsionDetMgr() { return Imp::cachedRetrieve(Imp::m_emulsionDetMgr,"Emulsion",VP1JobConfigInfo::hasEmulsionGeometry()); }

const ScintDD::VetoDetectorManager * VP1DetInfo::vetoDetMgr() { return Imp::cachedRetrieve(Imp::m_vetoDetMgr,"Veto",VP1JobConfigInfo::hasVetoGeometry()); }
const ScintDD::VetoNuDetectorManager * VP1DetInfo::vetoNuDetMgr() { return Imp::cachedRetrieve(Imp::m_vetoNuDetMgr,"VetoNu",VP1JobConfigInfo::hasVetoNuGeometry()); }
const ScintDD::TriggerDetectorManager * VP1DetInfo::triggerDetMgr() { return Imp::cachedRetrieve(Imp::m_triggerDetMgr,"Trigger",VP1JobConfigInfo::hasTriggerGeometry()); }
const ScintDD::PreshowerDetectorManager * VP1DetInfo::preshowerDetMgr() { return Imp::cachedRetrieve(Imp::m_preshowerDetMgr,"Preshower",VP1JobConfigInfo::hasPreshowerGeometry()); }

const TrackerDD::SCT_DetectorManager * VP1DetInfo::sctDetMgr() { return Imp::cachedRetrieve(Imp::m_sctDetMgr,"SCT",VP1JobConfigInfo::hasSCTGeometry()); }

const CaloDD::EcalDetectorManager * VP1DetInfo::ecalDetMgr() { return Imp::cachedRetrieve(Imp::m_ecalDetMgr,"Ecal",VP1JobConfigInfo::hasEcalGeometry()); }

const FaserDetectorID * VP1DetInfo::faserIDHelper() { return Imp::cachedRetrieve(Imp::m_faserIDHelper,"FaserID",true); }

const EmulsionID * VP1DetInfo::emulsionIDHelper() { return Imp::cachedRetrieve(Imp::m_emulsionIDHelper,"EmulsionID",VP1JobConfigInfo::hasEmulsionGeometry()); }

const VetoID * VP1DetInfo::vetoIDHelper() { return Imp::cachedRetrieve(Imp::m_vetoIDHelper,"VetoID",VP1JobConfigInfo::hasVetoGeometry()); }
const VetoNuID * VP1DetInfo::vetoNuIDHelper() { return Imp::cachedRetrieve(Imp::m_vetoNuIDHelper,"VetoNuID",VP1JobConfigInfo::hasVetoNuGeometry()); }
const TriggerID * VP1DetInfo::triggerIDHelper() { return Imp::cachedRetrieve(Imp::m_triggerIDHelper,"TriggerID",VP1JobConfigInfo::hasTriggerGeometry()); }
const PreshowerID * VP1DetInfo::preshowerIDHelper() { return Imp::cachedRetrieve(Imp::m_preshowerIDHelper,"PreshowerID",VP1JobConfigInfo::hasPreshowerGeometry()); }

const FaserSCT_ID * VP1DetInfo::sctIDHelper() { return Imp::cachedRetrieve(Imp::m_sctIDHelper,"FaserSCT_ID",VP1JobConfigInfo::hasSCTGeometry()); }

const EcalID * VP1DetInfo::ecalIDHelper() { return Imp::cachedRetrieve(Imp::m_ecalIDHelper,"EcalID",VP1JobConfigInfo::hasEcalGeometry()); }

//____________________________________________________________________
bool VP1DetInfo::isUnsafe( const Identifier& id ) {

  const FaserDetectorID * idhelper = faserIDHelper();
  if ( !idhelper || !id.is_valid() )
    return true;

  if (idhelper->is_neutrino(id))
  {
    if (!VP1JobConfigInfo::hasEmulsionGeometry() && idhelper->is_emulsion(id))
      return true;
  }

  if (idhelper->is_scint(id)) {
    if (!VP1JobConfigInfo::hasVetoGeometry() && idhelper->is_veto(id))
      return true;
    if (!VP1JobConfigInfo::hasVetoNuGeometry() && idhelper->is_vetonu(id))
      return true;
    if (!VP1JobConfigInfo::hasTriggerGeometry() && idhelper->is_trigger(id))
      return true;
    if (!VP1JobConfigInfo::hasPreshowerGeometry() && idhelper->is_preshower(id))
      return true;
  }

  if (idhelper->is_tracker(id)) {
    if (!VP1JobConfigInfo::hasSCTGeometry() && idhelper->is_sct(id))
      return true;
  }

  if (idhelper->is_calo(id)) {
    if (!VP1JobConfigInfo::hasEcalGeometry() && idhelper->is_ecal(id))
      return true;
  }

  return false;
}
