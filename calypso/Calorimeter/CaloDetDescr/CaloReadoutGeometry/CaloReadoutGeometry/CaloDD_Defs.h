/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CaloDD_Defs.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef CALOREADOUTGEOMETRY_CALODD_Defs_H
#define CALOREADOUTGEOMETRY_CALODD_Defs_H


namespace CaloDD {
  enum FrameType {local, global, other};
//   enum CarrierType {holes, electrons};
  // new enumerator to select given align-folder structure
  enum AlignFolderType {none = -1, static_run1 = 0, timedependent_run2 = 1};
}

#endif // CALOREADOUTGEOMETRY_CALODD_DEFS_H
