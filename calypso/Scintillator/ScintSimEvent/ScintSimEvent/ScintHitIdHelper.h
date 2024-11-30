/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCINTSIMEVENT_SCINTHITIDHELPER
#define SCINTSIMEVENT_SCINTHITIDHELPER

//
// This is a helper class to build an identifing integer used by
// the simulation. It inherits from HitIdHelper, in order to get
// all the packing and shifting for free.
// The class is a singleton and a static GetHelper() is provided
// the constructor calls the Initialize() method which sets all the
// field dimensions
// Methods are provided to get access to the ScintTracker Geometry
// description
//


//
// Base Class
#include "HitManagement/HitIdHelper.h"

// This class is singleton and static method and variable are used.
#include "CxxUtils/checker_macros.h"

//Helpers
#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"
#include "ScintIdentifier/VetoNuID.h"

#include "Identifier/Identifier.h"


ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

class ScintHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static ScintHitIdHelper* GetHelper();
  //
  // Info retrieval:
  // Veto or Trigger or Preshower
  bool isVeto(const int& hid) const;
  bool isTrigger(const int& hid) const;
  bool isPreshower(const int& hid) const;
  bool isVetoNu(const int& hid) const;

  // Barrel or Endcap
  int getStation(const int& hid) const;

  // Layer/Disk
  int getPlate(const int& hid) const;

  // identifier
  Identifier getIdentifier(const int& hid) const;

  //
  // Info packing:
  int buildHitId(const int, const int, const int) const;

 private:
  //
  // private constructor to have a singleton
  ScintHitIdHelper();
  //
  // Initialize the helper, only called by the constructor
  void Initialize();

  /// Detector ID helpers
  const VetoID* m_vetoID{nullptr};
  const TriggerID* m_triggerID{nullptr};
  const PreshowerID* m_preshowerID{nullptr};
  const VetoNuID* m_vetoNuID{nullptr};

};

#endif // SCINTSIMEVENT_SCINTHITIDHELPER
