/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "NeutrinoReadoutGeometry/NeutrinoCommonItems.h"

namespace NeutrinoDD {

NeutrinoCommonItems::NeutrinoCommonItems(const FaserDetectorID* const idHelper)
  : AthMessaging("NeutrinoDetectorElement")
  , m_idHelper(idHelper)
  , m_mutex{}
{}

// void   
// SiCommonItems::setSolenoidFrame(const HepGeom::Transform3D & transform) const
// {
//   std::lock_guard<std::mutex> lock{m_mutex};
//   m_solenoidFrame = transform;
// }

} // End namespace NeutrinoDD
