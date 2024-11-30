/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data associated with an TrackerRawData object
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: TrackerSimData.cxx,v 1.4 2004-03-01 22:07:07 costanzo Exp $


#include "TrackerSimData/TrackerSimData.h"
#include <iostream>



TrackerSimData::TrackerSimData() :
  m_word(0)
{
  
}

TrackerSimData::TrackerSimData (const std::vector<Deposit>& deposits, 
			    int simDataWord)
  : m_word((deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff))
{
  //above we pack deposits.size() and the simDataWord into a single unsigned int (m_word).
  //The first 3 bits are used for deposits.size() and the last 29 are used for the simDataWord.

  /*
    
  m_p_deposits = new Deposit[deposits.size()];
  
  for (unsigned int i = 0; i!= deposits.size(); i++)
  {
  m_p_deposits[i] = deposits[i];
  }
  */
  
  m_deposits = deposits;
}

TrackerSimData::TrackerSimData (std::vector<Deposit>&& deposits, 
			    int simDataWord)
  : m_deposits (std::move (deposits))
{
  m_word = (m_deposits.size()<<29 & 0xe0000000) | (simDataWord & 0x1fffffff);
}

TrackerSimData::TrackerSimData (const TrackerSimData& other)
  :m_word(other.m_word),
   m_deposits(other.m_deposits)
{

}

TrackerSimData &TrackerSimData::operator=(const TrackerSimData& other)
{
  if(&other != this) 
    { 
      m_word=other.m_word;
      m_deposits=other.m_deposits;
    }
  return *this;
}

TrackerSimData &TrackerSimData::operator=(TrackerSimData&& other)
{
  if(&other != this) 
    { 
      m_word=other.m_word;
      m_deposits=std::move(other.m_deposits);
    }
  return *this;
}

TrackerSimData::~TrackerSimData()
{
    //    delete[] m_p_deposits;
}
