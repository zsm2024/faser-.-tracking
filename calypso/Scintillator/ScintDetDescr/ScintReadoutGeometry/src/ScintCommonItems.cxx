/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "ScintReadoutGeometry/ScintCommonItems.h"

namespace ScintDD {

ScintCommonItems::ScintCommonItems(const FaserDetectorID* const idHelper)
  :  AthMessaging("ScintDetectorElement"),
     m_idHelper(idHelper), 
     m_mutex{}
{}

// void   
// SiCommonItems::setSolenoidFrame(const HepGeom::Transform3D & transform) const
// {
//   std::lock_guard<std::mutex> lock{m_mutex};
//   m_solenoidFrame = transform;
// }

} // End namespace ScintDD
