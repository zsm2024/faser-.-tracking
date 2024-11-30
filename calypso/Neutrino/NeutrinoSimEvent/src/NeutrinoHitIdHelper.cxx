/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <mutex>

#include "NeutrinoSimEvent/NeutrinoHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "NeutrinoIdentifier/EmulsionID.h"

#include "G4Types.hh"
#ifdef G4MULTITHREADED
#  include "GaudiKernel/ContextSpecificPtr.h"
#endif

static std::mutex sgMutex;

// This class is singleton and static method and variable are used.
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

//
// private constructor
NeutrinoHitIdHelper::NeutrinoHitIdHelper() :HitIdHelper() {
  Initialize();
}

NeutrinoHitIdHelper* NeutrinoHitIdHelper::GetHelper() {
#ifdef G4MULTITHREADED
  // Context-specific singleton
  static Gaudi::Hive::ContextSpecificPtr<NeutrinoHitIdHelper> helperPtr;
  if(!helperPtr) helperPtr = new NeutrinoHitIdHelper();
  return helperPtr.get();
#else
  static NeutrinoHitIdHelper helper;
  return &helper;
#endif
}

void NeutrinoHitIdHelper::Initialize() {

  // determine whether hits were created with an SLHC dictionary
  // in which case eta module field is expanded.
  // Need to lock this thread-unsafe retrieval
  const EmulsionID* pix;
  ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "NeutrinoHitIdHelper");
  if (detStore.retrieve().isSuccess()) {
    if (detStore->retrieve(pix, "EmulsionID").isFailure()) { pix = nullptr; }
  }
  if (pix == nullptr || pix->dictionaryVersion() == "FASERNU-03-770" || pix->dictionaryVersion() == "")
  {
    InitializeField("Module", 0, 34);
    InitializeField("Base", 0, 21);
  }
  else  // FASERNU-04-730
  {
    InitializeField("Module", 0, 72);
    InitializeField("Base", 0, 9);
  }
  InitializeField("Film", 0, 1);
}

// Module
int NeutrinoHitIdHelper::getModule(const int& hid) const
{
  return this->GetFieldValue("Module", hid);
}

// Base
int NeutrinoHitIdHelper::getBase(const int& hid) const
{
  return this->GetFieldValue("Base", hid);
}

// Film
int NeutrinoHitIdHelper::getFilm(const int& hid) const
{
  return this->GetFieldValue("Film", hid);
}

//
// Info packing:
int NeutrinoHitIdHelper::buildHitId( const int module, 
                                     const int base,
                                     const int film) const
{
  int theID(0);
  this->SetFieldValue("Module",  module,    theID);
  this->SetFieldValue("Base",    base,      theID);
  this->SetFieldValue("Film",    film,      theID);
  return theID;
}
