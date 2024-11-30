/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT1_RawData.h
//   Header file for class FaserSCT1_RawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to implement RawData for SCT, using method 1 for decoding word
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_FASERSCT1_RAWDATA_H
#define TRACKERRAWDATA_FASERSCT1_RAWDATA_H

// Base class
#include "TrackerRawData/FaserSCT_RDORawData.h"


// Data members classes



class FaserSCT1_RawData : public FaserSCT_RDORawData{

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline hash Id, last, word
  FaserSCT1_RawData(const Identifier rdoId, const unsigned int word);

  // Destructor:
  virtual ~FaserSCT1_RawData() = default;

  ///////////////////////////////////////////////////////////////////
  // Virtual methods 
  ///////////////////////////////////////////////////////////////////


  // decode group of strips
  virtual int getGroupSize() const;



  // Create a new FaserSCT1_RawData and return a pointer to it:
  //  static FaserSCT1_RawData *newObject(const Identifier rdoId, const unsigned int word);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  FaserSCT1_RawData();

  // Default copy constructor
  FaserSCT1_RawData(const FaserSCT1_RawData&) = default;
  // Default assignment operator
  FaserSCT1_RawData& operator=(const FaserSCT1_RawData&) = default;
  // Default move constructor
  FaserSCT1_RawData(FaserSCT1_RawData&&) = default;
  // Default move assignment operator
  FaserSCT1_RawData& operator=(FaserSCT1_RawData&&) = default;

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:


};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////
// decode size of group of strips information (from Calvet RawData class)
inline int FaserSCT1_RawData::getGroupSize() const
{
// Temporary mask, should be fixed later
  return (m_word & 0xFFFF);
}


#endif // TRACKERRAWDATA_SCT1_RAWDATA_H
