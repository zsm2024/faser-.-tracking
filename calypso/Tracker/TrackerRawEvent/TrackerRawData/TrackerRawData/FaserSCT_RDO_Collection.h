/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 TrackerRawData package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: Tracker_RDO_Collection.h,v 1.9 2007-05-28 12:47:23 kgnanvo Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef TRACKERRAWDATA_SCT_RDO_COLLECTION_H
#define TRACKERRAWDATA_SCT_RDO_COLLECTION_H

#include "AthenaKernel/CLASS_DEF.h"
#include "TrackerRawData/TrackerRawDataCollection.h"
#include "TrackerRawData/FaserSCT1_RawData.h"
#include "TrackerRawData/FaserSCT3_RawData.h"
class MsgStream;

typedef TrackerRawDataCollection< FaserSCT_RDORawData >
	FaserSCT_RDO_Collection; 

CLASS_DEF(FaserSCT_RDO_Collection, 1077697857, 1)

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const FaserSCT_RDO_Collection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const FaserSCT_RDO_Collection& coll);


// typedef InDetRawDataCollection< SCT_TB03_RawData >
// 	SCT_RDO_TB03_Collection; 

// CLASS_DEF(SCT_RDO_TB03_Collection, 1234089640, 1)

// typedef InDetRawDataCollection< SCT_TB04_RawData >
// 	SCT_RDO_TB04_Collection; 

// CLASS_DEF(SCT_RDO_TB04_Collection, 1278514121 , 1)

typedef TrackerRawDataCollection< FaserSCT3_RawData >
	FaserSCT3_RDO_Collection; 

CLASS_DEF(FaserSCT3_RDO_Collection, 1102069697 , 1)

// Class needed only for persistency - we add CLASS_DEF only to
// instantiate the template.
// typedef  DataVector<InDetRawDataCollection< SCT1_RawData > > SCT_RDO_vector; 

// CLASS_DEF( SCT_RDO_vector, 1241152074, 1 )

// typedef  DataVector<const InDetRawDataCollection< SCT1_RawData > > SCT_RDO_constvector; 

//CLASS_DEF( SCT_RDO_constvector, 1159798379, 1 )

#endif // TRACKERRAWDATA_FASERSCT_RDO_COLLECTION_H
