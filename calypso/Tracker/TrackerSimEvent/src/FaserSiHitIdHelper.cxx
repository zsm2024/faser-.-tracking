/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "TrackerSimEvent/FaserSiHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrackerIdentifier/FaserSCT_ID.h"

#include "G4Types.hh"
#ifdef G4MULTITHREADED
#  include "GaudiKernel/ContextSpecificPtr.h"
#endif

static std::mutex sgMutex;

// This class is singleton and static method and variable are used.
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

//
// private constructor
FaserSiHitIdHelper::FaserSiHitIdHelper() :HitIdHelper() {
  Initialize();
}

FaserSiHitIdHelper* FaserSiHitIdHelper::GetHelper() {
#ifdef G4MULTITHREADED
  // Context-specific singleton
  static Gaudi::Hive::ContextSpecificPtr<FaserSiHitIdHelper> helperPtr;
  if(!helperPtr) helperPtr = new FaserSiHitIdHelper();
  return helperPtr.get();
#else
  static FaserSiHitIdHelper helper;
  return &helper;
#endif
}

void FaserSiHitIdHelper::Initialize() {

  // in which case eta module field is expanded.
  // Need to lock this thread-unsafe retrieval
  const FaserSCT_ID* pix;
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "SiHitIdHelper");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->retrieve(pix, "FaserSCT_ID").isFailure()) { pix = 0; }
  }

  InitializeField("Station", 0, 3);
  InitializeField("Plane", 0, 2);
  InitializeField("Row", 0, 4);
  InitializeField("Module", -1, 1);
  InitializeField("Sensor", 0, 1);
}

// Info retrieval:
// Station
int FaserSiHitIdHelper::getStation(const int& hid) const
{
  return this->GetFieldValue("Station", hid);
}

// Plane
int FaserSiHitIdHelper::getPlane(const int& hid) const
{
  return this->GetFieldValue("Plane", hid);
}

// Row
int FaserSiHitIdHelper::getRow(const int& hid) const
{
  return this->GetFieldValue("Row", hid);
}

// Module
int FaserSiHitIdHelper::getModule(const int& hid) const
{
  return this->GetFieldValue("Module", hid);
}

// Sensor
int FaserSiHitIdHelper::getSensor(const int& hid) const
{
  return this->GetFieldValue("Sensor", hid);
}

//
// Info packing:
int FaserSiHitIdHelper::buildHitId(const int station, 
                                   const int plane,
                                   const int row,
                                   const int module,
                                   const int sensor) const
{
  int theID(0);
  this->SetFieldValue("Station",              station,                theID);
  this->SetFieldValue("Plane",                plane,                  theID);
  this->SetFieldValue("Row",                  row,                    theID);
  this->SetFieldValue("Module",               module,                 theID);
  this->SetFieldValue("Sensor",               sensor,                 theID);
  return theID;
}
