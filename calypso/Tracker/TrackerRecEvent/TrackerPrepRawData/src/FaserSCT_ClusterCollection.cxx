/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Tracker {
MsgStream& operator << ( MsgStream& sl, const FaserSCT_ClusterCollection& coll)
{
  sl << "FaserSCT_ClusterCollection: "
    << "identify()="<< coll.identify()
    //       << ", identifyHash()="<< coll.identifyHash()
    << ", PRD=[";
  FaserSCT_ClusterCollection::const_iterator it = coll.begin();
  FaserSCT_ClusterCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const FaserSCT_ClusterCollection& coll)
{
  sl << "FaserSCT_ClusterCollection: "
    << "identify()="<< coll.identify()
    //    << ", identifyHash()="<< coll.identifyHash()
    << ", PRD=[";
  FaserSCT_ClusterCollection::const_iterator it = coll.begin();
  FaserSCT_ClusterCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

}