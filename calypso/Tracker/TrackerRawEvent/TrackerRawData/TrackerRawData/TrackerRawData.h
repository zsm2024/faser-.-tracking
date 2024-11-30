/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerRawData.h
//   Header file for class TrackerRawData
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to handle raw data objects for Pixel and SCT and TRT
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_TRACKERRAWDATA_H
#define TRACKERRAWDATA_TRACKERRAWDATA_H

// Base class
#include "Identifier/Identifiable.h"
#include "Identifier/Identifier.h"
class MsgStream;


class TrackerRawData : public Identifiable {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  // offline compact identifier of the readout channel
  TrackerRawData(const Identifier rdoId, const unsigned int word);

  // Destructor:
  virtual ~TrackerRawData();

  ///////////////////////////////////////////////////////////////////
  // Virtual methods:
  ///////////////////////////////////////////////////////////////////

  virtual Identifier identify() const override final;


  virtual unsigned int getWord() const;

  ///////////////////////////////////////////////////////////////////
  // Clients should not use default constructor, rather use the one
  // above. It must be public for pool I/O.
  ///////////////////////////////////////////////////////////////////
  TrackerRawData();

  // OR the data word from OTHER into our data word.
  void merge (const TrackerRawData& other);

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  Identifier m_rdoId; //Offline ID for readout channel
  
protected:
  unsigned int m_word; // raw data word 
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const TrackerRawData& rdo);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const TrackerRawData& rdo);

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // TRACKERRAWDATA_TRACKERRAWDATA_H

