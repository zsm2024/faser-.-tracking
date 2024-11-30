/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloReadoutGeometry/CaloCommonItems.h"

namespace CaloDD {

CaloCommonItems::CaloCommonItems(const FaserDetectorID* const idHelper)
  :  AthMessaging("CaloDetectorElement"),
     m_idHelper(idHelper), 
     m_mutex{}
{}

} // End namespace CaloDD
