/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "ScintSimEvent/ScintHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"

#include "G4Types.hh"
#ifdef G4MULTITHREADED
#  include "GaudiKernel/ContextSpecificPtr.h"
#endif

static std::mutex sgMutex;

// This class is singleton and static method and variable are used.
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

//
// private constructor
ScintHitIdHelper::ScintHitIdHelper() :HitIdHelper() {
  Initialize();
}

ScintHitIdHelper* ScintHitIdHelper::GetHelper() {
#ifdef G4MULTITHREADED
  // Context-specific singleton
  static Gaudi::Hive::ContextSpecificPtr<ScintHitIdHelper> helperPtr;
  if(!helperPtr) helperPtr = new ScintHitIdHelper();
  return helperPtr.get();
#else
  static ScintHitIdHelper helper;
  return &helper;
#endif
}

void ScintHitIdHelper::Initialize() {

  // determine whether hits were created with an SLHC dictionary
  // in which case eta module field is expanded.
  // Need to lock this thread-unsafe retrieval
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "ScitHitIdHelper");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->retrieve(m_vetoID, "VetoID").isFailure()) { m_vetoID = 0; }
    if (detStore->retrieve(m_vetoNuID, "VetoNuID").isFailure()) { m_vetoNuID = 0; }
    if (detStore->retrieve(m_triggerID, "TriggerID").isFailure()) { m_triggerID = 0; }
    if (detStore->retrieve(m_preshowerID, "PreshowerID").isFailure()) { m_preshowerID = 0; }
  }

  InitializeField("VetoTriggerPreshower", 0, 3);
  InitializeField("Station", -2, 2);
  InitializeField("Plate", 0, 4);
}

// Info retrieval:
// Veto or Trigger or Preshower
bool ScintHitIdHelper::isVeto(const int& hid) const
{
  int ps = this->GetFieldValue("VetoTriggerPreshower", hid);
  if ( ps == 0 ) return true;
  else return false;
}

bool ScintHitIdHelper::isVetoNu(const int& hid) const
{
  int ps = this->GetFieldValue("VetoTriggerPreshower", hid);
  if ( ps == 3 ) return true;
  else return false;
}

bool ScintHitIdHelper::isTrigger(const int& hid) const
{
  int ps = this->GetFieldValue("VetoTriggerPreshower", hid);
  if ( ps == 1 ) return true;
  else return false;
}

bool ScintHitIdHelper::isPreshower(const int& hid) const
{
  int ps = this->GetFieldValue("VetoTriggerPreshower", hid);
  if ( ps == 2 ) return true;
  else return false;
}

// Station
int ScintHitIdHelper::getStation(const int& hid) const
{
  return this->GetFieldValue("Station", hid);
}

// Plate
int ScintHitIdHelper::getPlate(const int& hid) const
{
  return this->GetFieldValue("Plate", hid);
}


// identifier
Identifier ScintHitIdHelper::getIdentifier(const int& hid) const
{
  if (isVeto(hid)) {
    return m_vetoID->pmt_id(getStation(hid), getPlate(hid), 0);
  } else if (isTrigger(hid)) {
    return m_triggerID->pmt_id(getStation(hid), getPlate(hid), 0);
  } else if (isPreshower(hid)) {
    return m_preshowerID->pmt_id(getStation(hid), getPlate(hid), 0);
  } else if (isVetoNu(hid)) {
    return m_vetoNuID->pmt_id(getStation(hid), getPlate(hid), 0);
  }
  return Identifier();
}

//
// Info packing:
int ScintHitIdHelper::buildHitId(const int veto_trigger_preshower,
                                 const int station, 
                                 const int plate) const
{
  int theID(0);
  this->SetFieldValue("VetoTriggerPreshower", veto_trigger_preshower, theID);
  this->SetFieldValue("Station",              station,                theID);
  this->SetFieldValue("Plate",                plate,                  theID);
  return theID;
}
