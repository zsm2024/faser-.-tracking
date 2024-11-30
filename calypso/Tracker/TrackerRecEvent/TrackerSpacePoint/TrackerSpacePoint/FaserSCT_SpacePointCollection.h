/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT_SpacePointCollection.h
//   Header file for class FaserSCT_SpacePointCollection
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to contain all the SPs of a detector element:
// module for Pixel or wafer for SCT or layer for TRT, 
// for Muons:
// For SCT the DE is ambiguous, we are using the id of the R module (not the one with
// the stereo angle )
// The overlap space points are contained in the FaserSCT_SpacePointOverlapCollection
///////////////////////////////////////////////////////////////////
// Version 1.0 14/10/2003 Veronique Boisvert
///////////////////////////////////////////////////////////////////

#ifndef TRACKERSPACEPOINT_FASERSCT_SPACEPOINTCOLLECTION_H
#define TRACKERSPACEPOINT_FASERSCT_SPACEPOINTCOLLECTION_H

// Base classes
#include "Identifier/Identifier.h"
#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/DataObject.h"
//class SpacePoint;
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"


class FaserSCT_SpacePointCollectionCopyConstructorCalled{};


class FaserSCT_SpacePointCollection 
	: public Identifiable,
	public DataVector< Tracker::FaserSCT_SpacePoint >
{
	///////////////////////////////////////////////////////////////////
	// Public methods:
	///////////////////////////////////////////////////////////////////
	public:

        // Standard thinning code won't work on this type because
        // we have no default constructor.
        static bool constexpr supportsThinning = false;

        // Constructor with parameters:
	//   Hashed offline identifier of the DE
	FaserSCT_SpacePointCollection(const IdentifierHash idHash);
	
	// Destructor:
	virtual ~FaserSCT_SpacePointCollection();
	
	/**added for 10.0.0 as a hack for HLT. DO NOT USE IT. It will throw an exception*/
	FaserSCT_SpacePointCollection(const FaserSCT_SpacePointCollection& spc);
	
	
	///////////////////////////////////////////////////////////////////
	// Const methods:
	///////////////////////////////////////////////////////////////////

	virtual Identifier identify() const override final;
	
	virtual IdentifierHash identifyHash() const override final;
	
	void setIdentifier(Identifier id);

	virtual std::string type() const;
	
	///////////////////////////////////////////////////////////////////
	// Private methods:
	///////////////////////////////////////////////////////////////////
	private:
	
	FaserSCT_SpacePointCollection() = delete;
	FaserSCT_SpacePointCollection &operator=(const FaserSCT_SpacePointCollection&) = delete;
	
	///////////////////////////////////////////////////////////////////
	// Private data:
	///////////////////////////////////////////////////////////////////
	private:
	const IdentifierHash m_idHash; 
	Identifier m_id; // identifier of the DE
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const FaserSCT_SpacePointCollection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const FaserSCT_SpacePointCollection& coll);
///////////////////////////////////////////////////////////////////
// Inline methods:
///////////////////////////////////////////////////////////////////

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF(FaserSCT_SpacePointCollection,1129085627,1)

#endif // TRKSPACEPOINT_SPACEPOINTCOLLECTION_H
