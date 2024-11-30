/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT3_RawData.cxx
//   Implementation file for class FaserSCT3_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 11/12/2004 MJ Costa
///////////////////////////////////////////////////////////////////

#include <new>
#include "TrackerRawData/FaserSCT3_RawData.h"
#include "TrackerRawData/FaserSCT_RDORawData.h" 

// default constructor
FaserSCT3_RawData::FaserSCT3_RawData() :
  FaserSCT_RDORawData(Identifier(), 0) //call base-class constructor
{}

// Constructor with parameters:
FaserSCT3_RawData::FaserSCT3_RawData(const Identifier rdoId, const unsigned int word, const std::vector<int>* errorhit) :
  FaserSCT_RDORawData( rdoId, word), //call base-class constructor
  m_errorCondensedHit (*errorhit)
{
}

FaserSCT3_RawData::FaserSCT3_RawData(const Identifier rdoId,
                                     const unsigned int word,
                                     std::vector<int>&& errorhit)
  : FaserSCT_RDORawData( rdoId, word),
    m_errorCondensedHit (std::move(errorhit))
{
}
