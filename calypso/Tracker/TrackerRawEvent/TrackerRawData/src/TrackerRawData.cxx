/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerRawData.cxx
//   Implementation file for class TrackerRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrackerRawData/TrackerRawData.h"
#include "GaudiKernel/MsgStream.h"

TrackerRawData::TrackerRawData(): 
  m_word(0)
{}

// Constructor with parameters:
TrackerRawData::TrackerRawData(const Identifier rdoId, const unsigned int word) :
  Identifiable(),
  m_rdoId(rdoId),
  m_word(word)
{

}

// Destructor:
TrackerRawData::~TrackerRawData()
{

}

Identifier TrackerRawData::identify() const
{
  return m_rdoId;

}
unsigned int TrackerRawData::getWord() const
{
  return m_word;
}


void TrackerRawData::merge (const TrackerRawData& other)
{
  m_word |= other.m_word;
}


MsgStream& operator << ( MsgStream& sl, const TrackerRawData& rdo) {
  sl << " Identifier " << rdo.identify() << " word " << rdo.getWord();
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const TrackerRawData& rdo) {
  sl << " Identifier " << rdo.identify() << " word " << rdo.getWord();
  return sl;
}
