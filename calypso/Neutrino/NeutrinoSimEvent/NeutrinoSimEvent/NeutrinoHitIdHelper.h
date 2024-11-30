/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NEUTRINOSIMEVENT_NEUTRINOHITIDHELPER
#define NEUTRINOSIMEVENT_NEUTRINOHITIDHELPER

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
ATLAS_NO_CHECK_FILE_THREAD_SAFETY;

class NeutrinoHitIdHelper : HitIdHelper {
 public:
  //
  // Access to the helper
  static NeutrinoHitIdHelper* GetHelper();
  //

  // Front or back
  int getFilm(const int& hid) const;

  // Which base
  int getBase(const int& hid) const;

  // Which module
  int getModule(const int& hid) const;

  //
  // Info packing:
  int buildHitId(const int module, const int base, const int film) const;

 private:
  //
  // private constructor to have a singleton
  NeutrinoHitIdHelper();
  //
  // Initialize the helper, only called by the constructor
  void Initialize();
};

#endif // NEUTRINOSIMEVENT_NEUTRINOHITIDHELPER
