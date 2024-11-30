/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERRAWDATA_P2_H
#define TRACKERRAWDATA_P2_H

#include "Identifier/Identifier.h"

class TrackerRawData_p2 {
 public:
  TrackerRawData_p2():m_rdoId(0),m_word(0)  {};
// List of Cnv classes that convert this into Rdo objects
  friend class FaserSCT1_RawDataCnv_p2;
 private:
  Identifier::value_type m_rdoId; //Offline ID for readout channel
  unsigned int m_word; // raw data word 
};

#endif
