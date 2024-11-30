/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERPRD_COLLECTION_P1_H
#define TRACKERPRD_COLLECTION_P1_H


namespace Tracker{
class TrackerPRD_Collection_p1 
{
    
  
public:
  
    TrackerPRD_Collection_p1()
            : 
            m_id(0),
            m_hashId(0),
            m_begin(0),
            m_end(0)
        { } ;

    // container cnvs do conversions
    // friend class PixelClusterContainerCnv_p1;
    friend class FaserSCT_ClusterContainerCnv_p1;
    // friend class TRT_DriftCircleContainerCnv_p1;
    
    //private:
  
    // Identifier of this collection
    unsigned int m_id;
 
    //Identifier::value_type m_id;

    // Hash Identifier of this collection 
    // (write this one as well, so we don't rely on the IdHelper for it)   
    unsigned int m_hashId;    

    // Begin index into master collection
    unsigned int m_begin;

    // End index into master collection
    unsigned int m_end;
  
};

}

#endif


