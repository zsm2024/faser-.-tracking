/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT_RDORawData.cxx
//   Implementation file for class FaserSCT_RDORawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "TrackerRawData/FaserSCT_RDORawData.h"
#include "TrackerRawData/TrackerRawData.h"

// Constructor with parameters:
FaserSCT_RDORawData::FaserSCT_RDORawData(const Identifier rdoId, 
			   const unsigned int word) :
  TrackerRawData(rdoId, word) //call base class constructor
{}
