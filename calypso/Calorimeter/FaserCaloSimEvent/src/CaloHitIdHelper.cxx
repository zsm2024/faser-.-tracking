/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "FaserCaloSimEvent/CaloHitIdHelper.h"
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
CaloHitIdHelper::CaloHitIdHelper() :HitIdHelper() {
  Initialize();
}

CaloHitIdHelper* CaloHitIdHelper::GetHelper() {
#ifdef G4MULTITHREADED
  // Context-specific singleton
  static Gaudi::Hive::ContextSpecificPtr<CaloHitIdHelper> helperPtr;
  if(!helperPtr) helperPtr = new CaloHitIdHelper();
  return helperPtr.get();
#else
  static CaloHitIdHelper helper;
  return &helper;
#endif
}

void CaloHitIdHelper::Initialize() {

  // determine whether hits were created with an SLHC dictionary
  // in which case eta module field is expanded.
  // Need to lock this thread-unsafe retrieval
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "CaloHitIdHelper");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->retrieve(m_ecalID, "EcalID").isFailure()) { m_ecalID = 0; }
  }

  InitializeField("Row", 0, 2);
  InitializeField("Module", 0, 2);  // Need three values for test beam
}

// Station
int CaloHitIdHelper::getRow(const int& hid) const
{
  return this->GetFieldValue("Row", hid);
}

// Plate
int CaloHitIdHelper::getModule(const int& hid) const
{
  return this->GetFieldValue("Module", hid);
}

// identifier
Identifier CaloHitIdHelper::getIdentifier(const int& hid) const
{
  return m_ecalID->pmt_id(getRow(hid), getModule(hid), 0);

}


//
// Info packing:
int CaloHitIdHelper::buildHitId( const int row, 
                                 const int module) const
{
  int theID(0);
  this->SetFieldValue("Row",     row,    theID);
  this->SetFieldValue("Module",  module, theID);
  return theID;
}
