/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerDD_Defs.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRACKERREADOUTGEOMETRY_TRACKERDD_Defs_H
#define TRACKERREADOUTGEOMETRY_TRACKERDD_Defs_H


namespace TrackerDD {
  enum FrameType {local, global, other};
  enum CarrierType {holes, electrons};
  // new enumerator to select given align-folder structure
  enum AlignFolderType {none = -1, static_run1 = 0, timedependent_run2 = 1};
}

#endif // TRACKERREADOUTGEOMETRY_TRACKERDD_DEFS_H
