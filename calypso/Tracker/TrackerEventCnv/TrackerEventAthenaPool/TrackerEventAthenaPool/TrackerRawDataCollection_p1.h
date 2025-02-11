/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERRAWDATACOLLECTION_P1_H
#define TRACKERRAWDATACOLLECTION_P1_H

#include <vector>
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"

class TrackerRawDataCollection_p1 
{
    
  
public:
  
    TrackerRawDataCollection_p1() 
            : 
            m_id(0),
            m_hashId(0),
            m_begin(0),
            m_end(0)
        { } ;

    // container cnv does conversion
    friend class TrackerRawDataContainerCnv_p1;
    
    //private:
  
    // Identifier of this collection
    Identifier32::value_type m_id;

    // Hash Identifier of this collection 
    // (write this one as well, so we don't rely on the IdHelper for it)   
    IdentifierHash::value_type m_hashId;    

    // Begin index into master collection
    unsigned int m_begin;

    // End index into master collection
    unsigned int m_end;
  
};

#endif
