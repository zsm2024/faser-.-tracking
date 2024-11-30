/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Common flags shared by several subsystems                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDCOMMONFLAGS_H
#define PRDCOMMONFLAGS_H

#include <QFlags>

class PRDCommonFlags {
public:

  enum TrackerPartsFlag {
    None           = 0x000000,
    Interface      = 0x000001,
    Upstream       = 0x000002,
    Central        = 0x000004,
    Downstream     = 0x000008,
    AllBaseline    = 0x00000E,
    All            = 0x00000F
  };
  Q_DECLARE_FLAGS(TrackerPartsFlags, TrackerPartsFlag)

private:
  PRDCommonFlags();
  ~PRDCommonFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(PRDCommonFlags::TrackerPartsFlags)

#endif
