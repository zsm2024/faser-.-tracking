/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 CLASS_DEF for TrackerSimData map
 ------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: TrackerSimDataCollection.h,v 1.3 2004-04-05 23:16:07 costanzo Exp $

#ifndef TRACKERSIMDATA_TRACKERSIMDATACOLLECTION_H
# define TRACKERSIMDATA_TRACKERSIMDATACOLLECTION_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "AthenaKernel/CLASS_DEF.h"
#include "TrackerSimData/TrackerSimData.h"
#include "Identifier/Identifier.h"
#include <map>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class TrackerSimDataCollection : public std::map<Identifier,TrackerSimData> {
 public:
  TrackerSimDataCollection();
  virtual ~TrackerSimDataCollection();
};
CLASS_DEF(TrackerSimDataCollection, 1180099465, 1)


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // TRACKERSIMDATA_TRACKERSIMDATACOLLECTION_H