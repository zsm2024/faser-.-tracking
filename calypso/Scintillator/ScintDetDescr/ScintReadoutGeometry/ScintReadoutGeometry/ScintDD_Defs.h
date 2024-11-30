/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ScintDD_Defs.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef SCINTREADOUTGEOMETRY_SCINTDD_Defs_H
#define SCINTREADOUTGEOMETRY_SCINTDD_Defs_H


namespace ScintDD {
  enum FrameType {local, global, other};
//   enum CarrierType {holes, electrons};
  // new enumerator to select given align-folder structure
  enum AlignFolderType {none = -1, static_run1 = 0, timedependent_run2 = 1};
}

#endif // SCINTREADOUTGEOMETRY_SCINTDD_DEFS_H
