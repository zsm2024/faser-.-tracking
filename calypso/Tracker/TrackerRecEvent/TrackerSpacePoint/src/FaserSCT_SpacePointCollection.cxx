/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SpacePointCollection.cxx
//   Implementation file for class SpacePointCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#include "TrackerSpacePoint/FaserSCT_SpacePointCollection.h"
#include "GaudiKernel/MsgStream.h"
// Constructor with parameters:
FaserSCT_SpacePointCollection::FaserSCT_SpacePointCollection(const IdentifierHash idHash) :
  m_idHash(idHash),
  m_id()
{}

// Constructor with parameters:
FaserSCT_SpacePointCollection::FaserSCT_SpacePointCollection(const FaserSCT_SpacePointCollection& spc) :
  DataVector< Tracker::FaserSCT_SpacePoint >( spc ),
  m_idHash(),
  m_id()
{
	// added this ctor as a temporary hack for HLT
	// it should never be called.
	throw FaserSCT_SpacePointCollectionCopyConstructorCalled();
}

// Destructor:
FaserSCT_SpacePointCollection::~FaserSCT_SpacePointCollection()
{ }

Identifier FaserSCT_SpacePointCollection::identify() const
{
  return m_id;
}

IdentifierHash FaserSCT_SpacePointCollection::identifyHash() const
{
  return m_idHash;
}

void FaserSCT_SpacePointCollection::setIdentifier(Identifier id)
{
  m_id = id;
}

std::string FaserSCT_SpacePointCollection::type() const
{
  return "FaserSCT_SpacePointCollection";
}
/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const FaserSCT_SpacePointCollection& coll) {
  sl << "FaserSCT_SpacePointCollection: "
     << "identify()="<< coll.identify()
     << ", SP=[";
  FaserSCT_SpacePointCollection::const_iterator it = coll.begin();
  FaserSCT_SpacePointCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) { sl<< (**it)<<", ";}
  sl <<" ]"<<std::endl;
  return sl;
}

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const FaserSCT_SpacePointCollection& coll) {
  sl << "FaserSCT_SpacePointCollection: "
     << "identify()="<< coll.identify()
     << ", SP=[";
  FaserSCT_SpacePointCollection::const_iterator it = coll.begin();
  FaserSCT_SpacePointCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) { sl<< (**it)<<", ";}
  sl <<" ]"<<std::endl;
  return sl;
}
