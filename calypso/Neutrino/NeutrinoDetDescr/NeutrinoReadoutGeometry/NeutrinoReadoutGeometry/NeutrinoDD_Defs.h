/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// NeutrinoDD_Defs.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef NEUTRINOREADOUTGEOMETRY_NEUTRINODD_Defs_H
#define NEUTRINOREADOUTGEOMETRY_NEUTRINODD_Defs_H


namespace NeutrinoDD {
  enum FrameType {local, global, other};
//   enum CarrierType {holes, electrons};
  // new enumerator to select given align-folder structure
  enum AlignFolderType {none = -1, static_run1 = 0, timedependent_run2 = 1};
}

#endif // NEUTRINOREADOUTGEOMETRY_NEUTRINODD_DEFS_H
