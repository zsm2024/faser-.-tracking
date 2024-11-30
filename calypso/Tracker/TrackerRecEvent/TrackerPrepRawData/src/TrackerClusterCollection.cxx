/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerPrepRawData/TrackerClusterCollection.h"
#include "GaudiKernel/MsgStream.h"

namespace Tracker {

MsgStream& operator << ( MsgStream& sl, const TrackerClusterCollection& coll)
{
  sl << "TrackerClusterCollection: "
    << "identify()="<< coll.identify()
    //       << ", identifyHash()="<< coll.identifyHash()
    << ", PRD=[";
  TrackerClusterCollection::const_iterator it = coll.begin();
  TrackerClusterCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const TrackerClusterCollection& coll)
{
  sl << "TrackerClusterCollection: "
    << "identify()="<< coll.identify()
    //    << ", identifyHash()="<< coll.identifyHash()
    << ", PRD=[";
  TrackerClusterCollection::const_iterator it = coll.begin();
  TrackerClusterCollection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

}