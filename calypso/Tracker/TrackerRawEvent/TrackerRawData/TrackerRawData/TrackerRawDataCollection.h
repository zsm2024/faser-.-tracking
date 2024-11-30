/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TrackerRawDataCollection.h
//   Header file for class TrackerRawDataCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the RawDataObject of a detector element:
// wafer for SCT
///////////////////////////////////////////////////////////////////
// Version 1.0 13/08/2002 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_TRACKERRAWDATACOLLECTION_H
#define TRACKERRAWDATA_TRACKERRAWDATACOLLECTION_H

// Base classes
#include "Identifier/Identifiable.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"


// Needed class
#include "TrackerRawData/TrackerRawData.h"

template< class RawDataT >
class TrackerRawDataCollection : public Identifiable, public DataVector< RawDataT > {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
public:

  // Constructor with parameters:
  //   Hashed offline identifier of the DE
  TrackerRawDataCollection(const IdentifierHash idHash);

  // Destructor:
  virtual ~TrackerRawDataCollection();


  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  virtual Identifier identify() const override final;

  virtual IdentifierHash identifyHash() const override final;

  void setIdentifier(Identifier id);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
private:

  TrackerRawDataCollection(const TrackerRawDataCollection&);
  TrackerRawDataCollection &operator=(const TrackerRawDataCollection&);

public:
  // public default constructor needed for I/O, but should not be
  // called from an alg
  TrackerRawDataCollection();

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
private:
  const IdentifierHash m_idHash;
  Identifier m_id; // identifier of the DE
};

/**Overload of << operator for MsgStream for debug output*/
template< class RawDataT >
MsgStream& operator << ( MsgStream& sl, const TrackerRawDataCollection< RawDataT >& coll);

/**Overload of << operator for std::ostream for debug output*/ 
template< class RawDataT >
std::ostream& operator << ( std::ostream& sl, const TrackerRawDataCollection< RawDataT >& coll);

// member functions that use Collection T
#include"TrackerRawData/TrackerRawDataCollection.icc"
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#endif // TRACKERRAWDATA_TRACKERRAWDATACOLLECTION_H
