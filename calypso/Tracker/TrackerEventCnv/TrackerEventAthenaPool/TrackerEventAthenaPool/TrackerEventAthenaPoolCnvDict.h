/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKEREVENTATHENAPOOLCNVDICT_H
#define TRACKEREVENTATHENAPOOLCNVDICT_H

// #include "TrackerEventAthenaPool/TrackerSimData_p1.h"
#include "TrackerEventAthenaPool/TrackerSimData_p2.h"
// #include "TrackerEventAthenaPool/TrackerSimDataCollection_p1.h"
// #include "TrackerEventAthenaPool/TrackerSimDataCollection_p2.h"
#include "TrackerEventAthenaPool/TrackerSimDataCollection_p3.h"
#include "TrackerEventAthenaPool/TrackerRawData_p1.h"
// #include "TrackerEventAthenaPool/FaserSCT3_RawData_p1.h"
// #include "TrackerEventAthenaPool/FaserSCT3_RawData_p2.h"
// #include "TrackerEventAthenaPool/FaserSCT3_RawData_p3.h"
#include "TrackerEventAthenaPool/FaserSCT3_RawData_p4.h"
#include "TrackerEventAthenaPool/TrackerRawDataCollection_p1.h"
// #include "TrackerEventAthenaPool/TrackerRawDataContainer_p1.h"
#include "TrackerEventAthenaPool/TrackerRawDataContainer_p2.h"
// #include "TrackerEventAthenaPool/FaserSCT_RawDataContainer_p1.h"
// #include "TrackerEventAthenaPool/FaserSCT_RawDataContainer_p2.h"
// #include "TrackerEventAthenaPool/FaserSCT_RawDataContainer_p3.h"
#include "TrackerEventAthenaPool/FaserSCT_RawDataContainer_p4.h"
// #include "TrackerEventAthenaPool/FaserSCT_FlaggedCondData_p1.h"

namespace TrackerEventAthenaPoolCnvDict
{
    std::pair<unsigned int, TrackerSimData_p2> t1;
    std::pair<unsigned long long, TrackerSimData_p2> t2;
    std::vector<std::pair<unsigned int, TrackerSimData_p2> > t3;
    std::vector<std::pair<unsigned long long, TrackerSimData_p2> > t4;
}

#endif
