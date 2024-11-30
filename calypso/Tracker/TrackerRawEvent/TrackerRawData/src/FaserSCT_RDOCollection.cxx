/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrackerRawData/FaserSCT_RDO_Collection.h"
#include "GaudiKernel/MsgStream.h"

MsgStream& operator << ( MsgStream& sl, const FaserSCT_RDO_Collection& coll)
{
  sl << "FaserSCT_RDO_Collection: "
     << "identify()="<< coll.identify()
    //       << ", identifyHash()="<< coll.identifyHash()
     << ", RDO=[";
  FaserSCT_RDO_Collection::const_iterator it = coll.begin();
  FaserSCT_RDO_Collection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}

std::ostream& operator << ( std::ostream& sl, const FaserSCT_RDO_Collection& coll)
{
  sl << "FaserSCT_RDO_Collection: "
     << "identify()="<< coll.identify()
    //    << ", identifyHash()="<< coll.identifyHash()
     << ", RDO=[";
  FaserSCT_RDO_Collection::const_iterator it = coll.begin();
  FaserSCT_RDO_Collection::const_iterator itEnd = coll.end();
  for (;it!=itEnd;++it) sl<< (**it)<<", ";
  sl <<" ]"<<std::endl;
  return sl;
}
