/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Common flags used in the raw data system.                 //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef VP1RAWDATAFLAGS_H
#define VP1RAWDATAFLAGS_H

#include <QFlags>

class VP1RawDataFlags {
public:

  enum TrackerPartsFlag {
    None           = 0x000000,
    Upstream       = 0x000001,
    Central        = 0x000002,
    Downstream     = 0x000004,
    Interface      = 0x000008,
    All            = 0x00000F
  };
  Q_DECLARE_FLAGS(TrackerPartsFlags, TrackerPartsFlag)

private:
  VP1RawDataFlags();
  ~VP1RawDataFlags();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(VP1RawDataFlags::TrackerPartsFlags)

#endif
