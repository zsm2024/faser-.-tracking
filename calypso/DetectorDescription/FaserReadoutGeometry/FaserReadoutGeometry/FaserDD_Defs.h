/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserDD_Defs.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERREADOUTGEOMETRY_FASERDD_Defs_H
#define FASERREADOUTGEOMETRY_FASERDD_Defs_H


namespace FaserDD {
  enum FrameType {local, global, other};
  enum CarrierType {holes, electrons};
  // new enumerator to select given align-folder structure
  enum AlignFolderType {none = -1, static_run1 = 0, timedependent_run2 = 1};
}

#endif // FASERREADOUTGEOMETRY_FASERDD_DEFS_H
