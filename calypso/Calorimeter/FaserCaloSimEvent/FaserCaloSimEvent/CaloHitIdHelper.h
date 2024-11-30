/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENT_CALOHITIDHELPER
#define CALOSIMEVENT_CALOHITIDHELPER

//
// This is a helper class to build an identifing integer used by
// the simulation. It inherits from HitIdHelper, in order to get
// all the packing and shifting for free.
// The class is a singleton and a static GetHelper() is provided
// the constructor calls the Initialize() method which sets all the
// field dimensions
// Methods are provided to get access to the Geometry
// description
//


//
// Base Class
#include "HitManagement/HitIdHelper.h"

// This class is singleton and static method and variable are used.
#include "CxxUtils/checker_macros.h"

#include "Identifier/Identifier.h"

#include "FaserCaloIdentifier/EcalID.h"

ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

class CaloHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static CaloHitIdHelper* GetHelper();
  //
  // Info retrieval:
  // Veto or Trigger or Preshower
  // bool isVeto(const int& hid) const;
  // bool isTrigger(const int& hid) const;
  // bool isPreshower(const int& hid) const;

  // Top or Bottom
  int getRow(const int& hid) const;

  // Left or Right
  int getModule(const int& hid) const;

  Identifier getIdentifier(const int& hid) const;

  //
  // Info packing:
  int buildHitId(const int, const int) const;

 private:
  //
  // private constructor to have a singleton
  CaloHitIdHelper();
  //
  // Initialize the helper, only called by the constructor
  void Initialize();

  /// Detector ID helper
  const EcalID* m_ecalID{nullptr};
};

#endif // CALOSIMEVENT_CALOHITIDHELPER
