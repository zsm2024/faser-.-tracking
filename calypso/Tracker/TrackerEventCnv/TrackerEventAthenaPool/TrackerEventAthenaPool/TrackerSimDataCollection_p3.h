/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMDATACOLLECTION_P3_H
#define TRACKERSIMDATACOLLECTION_P3_H
                                                                                                                                                             
#include <vector>
#include "TrackerEventAthenaPool/TrackerSimData_p2.h"
#include "Identifier/Identifier.h"

class TrackerSimDataCollection_p3
{
                                                                                                                                                             
                                                                                                                                                             
public:
                                                                                                                                                             
    TrackerSimDataCollection_p3()
        { } ;
                                                                                                                                                             
    // container cnv does conversion
    friend class TrackerSimDataCollectionCnv_p3;
                                                                                                                                                             
private:
    std::vector<std::pair<Identifier::value_type, TrackerSimData_p2> > m_simdata;
};
                                                                                                                                                             
#endif
