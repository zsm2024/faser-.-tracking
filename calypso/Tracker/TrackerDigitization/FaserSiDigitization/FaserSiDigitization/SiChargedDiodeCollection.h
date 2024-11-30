/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiChargedDiodeCollection.h
//   Header file for class SiChargedDiodeCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class used to store the list of SiChargedDiode objects for one
//  module (Pixel) or half module (SCT) and one event. 
///////////////////////////////////////////////////////////////////
// Version 3.0 09/07/2001 David Calvet
// Davide Costanzo. Revisited version. 04-03-03
//    - added a Identifier wafer_id to the private members
//    - added a IdHelper to the private members
//    - constructor modified to initialize wafer_id and IdHelper
//    - replaced <list> with <map> and use the compact id of the 
//      SiChargedDiode to map them.
//    - Inherit from Identifiable to enforce the identify() method
///////////////////////////////////////////////////////////////////
#ifndef FASERSIDIGITIZATION_SICHARGEDDIODECOLLECTION_H
#define FASERSIDIGITIZATION_SICHARGEDDIODECOLLECTION_H

// Base class
#include "Identifier/Identifiable.h"

// Data member classes
#include <unordered_map>
#include "FaserSiDigitization/SiChargedDiode.h"
#include "Identifier/Identifier.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

// Input/output classes 
#include "TrackerSimEvent/FaserSiHit.h"

// STL includes
#include <atomic>
#include <set>
#include <memory>

class FaserDetectorID;
namespace TrackerDD{
  class SiDetectorElement;
  class SiDetectorDesign;
  class SiCellId;
}

#include "AthAllocators/ArenaPoolSTLAllocator.h"

struct SiChargedDiodeHash
{
  size_t operator() (const TrackerDD::SiCellId & id) const
  {
    return id.word();
  }
};

typedef std::unordered_map<TrackerDD::SiCellId,
                           SiChargedDiode,
                           SiChargedDiodeHash,
                           std::equal_to<TrackerDD::SiCellId>,
                           SG::ArenaPoolSTLAllocator<
                            std::pair<const TrackerDD::SiCellId, SiChargedDiode> > >
  SiChargedDiodeMap;


// Iterator typedef to make it easier
typedef SiChargedDiodeMap::iterator SiChargedDiodeIterator;



//
// A normal iteration over a SiChargedDiodeCollection will use
// the unordered_map, so the ordering is not defined.  The observed
// ordering can (and does) change depending on the compiler and library
// version used.  In some cases, though, we are sensitive to the
// order of iteration, for example in cases where we generate a random
// number for each element of the collection.  To get results which
// are identical across compilers, we need to instead do the iteration
// in a well-defined order.
//
// This can be done using the methods sortedBegin and sortedEnd instead
// of begin and end.  These work by maintaining a std::set of pointers
// to the diodes, sorted by the diode number.  In order to avoid paying
// the penalty for maintaining the sorted set when we don't need to, we only
// start maintaining it the first time that it's requested.
struct SiChargedDiodeOrderedSetCompare
{
  size_t operator() (const SiChargedDiode* a,
                     const SiChargedDiode* b) const
  {
    return a->diode().word() < b->diode().word();
  }
};



typedef std::set<SiChargedDiode*,
                 SiChargedDiodeOrderedSetCompare,
                 SG::ArenaPoolSTLAllocator<SiChargedDiode*> >
  SiChargedDiodeOrderedSet;
                 

// Iterator typedef to make it easier
typedef SiChargedDiodeOrderedSet::iterator SiChargedDiodeOrderedIterator;

class SiChargedDiodeCollection : Identifiable {
  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
 public:


  // Constructor with parameters:
  //  ref. to the detector element for this collection
  SiChargedDiodeCollection( );

  SiChargedDiodeCollection(const TrackerDD::SiDetectorElement* );


  // Destructor:
  ~SiChargedDiodeCollection();

  ///////////////////////////////////////////////////////////////////
  // Const methods:
  ///////////////////////////////////////////////////////////////////

  // detector element:
  const TrackerDD::SiDetectorElement * element() const;

  // wafer identifier for this collection
  virtual Identifier identify() const override final;
  virtual IdentifierHash identifyHash() const override final;

  // id helper for this collection
  const FaserDetectorID* id_helper();
  
  // detector design:
  const TrackerDD::SiDetectorDesign &design() const;

  // translation from SiReadoutCellId to Identifier
  Identifier getId(const TrackerDD::SiCellId& id) const 
  { 
    return (element()->identifierFromCellId(id)); 
  }

  ///////////////////////////////////////////////////////////////////
  // Non-const methods:
  ///////////////////////////////////////////////////////////////////

  // clean up:
  void clear();

  // read/write access to the collection:
  SiChargedDiodeMap &chargedDiodes();

  // Set the SiDetectorElement
  void setDetectorElement(const TrackerDD::SiDetectorElement *SiElement);

  // Add a new SiCharge to the collection
  // (add or merge in an existing SiChargedDiode):
  //   Diode which contains the new charge
  //   SiCharge to be added. 
  void add(const TrackerDD::SiCellId & diode, const SiCharge &charge);
  void add(const TrackerDD::SiCellId & diode, const SiTotalCharge &totcharge);

  bool AlreadyHit(const TrackerDD::SiCellId & siId);
  bool AlreadyHit(const Identifier & id);
  SiChargedDiodeIterator begin();
  SiChargedDiodeIterator end();
  SiChargedDiodeOrderedIterator orderedBegin();
  SiChargedDiodeOrderedIterator orderedEnd();
  bool empty() const; // Test if there is anything in the collection.

  // return a Charged diode given its CellId, NULL if doesn't exist
  SiChargedDiode * find(const TrackerDD::SiCellId & siId);
  // return a Charged diode given its identifier, NULL if doesn't exist
  SiChargedDiode * find(Identifier);

  ///////////////////////////////////////////////////////////////////
  // Private methods:
  ///////////////////////////////////////////////////////////////////
 private:
  SiChargedDiodeCollection(const SiChargedDiodeCollection&);
  SiChargedDiodeCollection &operator=(const SiChargedDiodeCollection&);
  void order();
  
  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:
  //NB m_allocator should always be declared before m_chargedDiodes in
  //the intialization list.  If the allocator is declared after
  //m_chargedDiodes, when the collection is destroyed, the allocator
  //will be destroyed (and the memory it manages freed) before the
  //SiChargedDiodeMap.  This will cause a crash unless the
  //SiChargedDiodeMap is empty.
  SiTotalCharge::alloc_t m_allocator; 
  SiChargedDiodeMap m_chargedDiodes; // list of SiChargedDiodes 
  SiChargedDiodeOrderedSet m_orderedChargedDiodes; // list of SiChargedDiodes 
  const TrackerDD::SiDetectorElement* m_sielement; // detector element
};

///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

// Set the DetectorElement
inline void SiChargedDiodeCollection::setDetectorElement(const TrackerDD::SiDetectorElement *SiElement) 
{
  m_sielement=SiElement;
}

inline SiChargedDiodeMap &SiChargedDiodeCollection::chargedDiodes()
{
  return m_chargedDiodes;
}

// access to the element
inline const TrackerDD::SiDetectorElement *SiChargedDiodeCollection::element() const
{
  return m_sielement;
}

// access to the design
inline const TrackerDD::SiDetectorDesign &SiChargedDiodeCollection::design() const
{
  return m_sielement->design();
}

// Get the Identifier of the collection
inline Identifier SiChargedDiodeCollection::identify() const
{
  return m_sielement->identify();
}

// Get the Identifier of the collection
inline IdentifierHash SiChargedDiodeCollection::identifyHash() const
{
  return m_sielement->identifyHash();
}

// Get the Id Helper
inline const FaserDetectorID* SiChargedDiodeCollection::id_helper()
{   
  return m_sielement->getIdHelper();
}


inline SiChargedDiodeIterator SiChargedDiodeCollection::begin() 
{
  return m_chargedDiodes.begin();
}

inline SiChargedDiodeIterator SiChargedDiodeCollection::end() 
{
  return m_chargedDiodes.end();
}

inline SiChargedDiodeOrderedIterator SiChargedDiodeCollection::orderedBegin() 
{
  if (m_orderedChargedDiodes.empty() && !m_chargedDiodes.empty()) {
    order();
  }
  return m_orderedChargedDiodes.begin();
}

inline SiChargedDiodeOrderedIterator SiChargedDiodeCollection::orderedEnd() 
{
  if (m_orderedChargedDiodes.empty() && !m_chargedDiodes.empty()) {
    order();
  }
  return m_orderedChargedDiodes.end();
}

inline bool SiChargedDiodeCollection::empty() const {
  return m_chargedDiodes.empty();
}



#endif // FASERSIDIGITIZATION_SICHARGEDDIODECOLLECTION_H

