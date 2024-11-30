/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_RAWDATACONTAINER_P4_H
#define FASERSCT_RAWDATACONTAINER_P4_H

/**

* Persistent represenation of an FaserSCT_RawDataContainer.
* New version to deal with FaserSCT_RawData_p4
* Susumu Oda Oct 2018.

*/

#include <vector>
#include <string>
#include "TrackerEventAthenaPool/FaserSCT3_RawData_p4.h"
#include "TrackerEventAthenaPool/TrackerRawData_p2.h"
#include "TrackerEventAthenaPool/TrackerRawDataCollection_p1.h"


class FaserSCT_RawDataContainer_p4
{
 public:
/// Default constructor
  FaserSCT_RawDataContainer_p4();
  friend class FaserSCT_RawDataContainerCnv_p4;
  //private:
  std::vector<TrackerRawDataCollection_p1>  m_collections;
  std::vector<TrackerRawData_p2>            m_rawdata;
  std::vector<FaserSCT3_RawData_p4>         m_sct3data;
  /** because ROOT isn't good at storing vectors of vectors,
   *  store the individual strip errors in the persistent container
   *  rather than in the persistent SCT3_RawData_p4 itself..
   */
};


/** inlines */

inline
FaserSCT_RawDataContainer_p4::FaserSCT_RawDataContainer_p4() {}

#endif // FASERSCT_RAWDATACONTAINER_P4_H
