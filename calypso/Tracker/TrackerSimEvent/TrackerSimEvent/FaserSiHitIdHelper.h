/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMEVENT_FASERSIHITIDHELPER
#define TRACKERSIMEVENT_FASERSIHITIDHELPER

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
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

class FaserSiHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static FaserSiHitIdHelper* GetHelper();
  //
  // Info retrieval:

  // Station
  int getStation(const int& hid) const;

  // Plane
  int getPlane(const int& hid) const;

  // Row
  int getRow(const int& hid) const;

  // Module
  int getModule(const int& hid) const;

  // Sensor
  int getSensor(const int& hid) const;

  //
  // Info packing:
  int buildHitId(const int, const int, const int, const int, const int) const;

 private:
  //
  // private constructor to have a singleton
  FaserSiHitIdHelper();
  //
  // Initialize the helper, only called by the constructor
  void Initialize();
};

#endif // TRACKERSIMEVENT_FASERSIHITIDHELPER
