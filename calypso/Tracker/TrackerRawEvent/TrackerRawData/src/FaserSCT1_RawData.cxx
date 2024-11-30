/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT1_RawData.cxx
//   Implementation file for class FaserSCT1_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include <new>
#include "TrackerRawData/FaserSCT1_RawData.h"
#include "TrackerRawData/FaserSCT_RDORawData.h"

// default constructor
FaserSCT1_RawData::FaserSCT1_RawData() :
  FaserSCT_RDORawData(Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
FaserSCT1_RawData::FaserSCT1_RawData(const Identifier rdoId, 
			   const unsigned int word) :
  FaserSCT_RDORawData( rdoId, word) //call base-class constructor
{}


//SCT1_RawData *SCT1_RawData::newObject(const Identifier rdoId, const unsigned int word)
//{
//  SCT1_RawData *p_rdo=0;

  // create a new object and check
//  try {
//    p_rdo=new SCT1_RawData(rdoId, word);
//  } catch (...) {
//    p_rdo=0;
//  }
//  return p_rdo;
//}

