/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LVNotifier.h"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"

#include "G4FaserGeometryNotifierSvc.h"

LVNotifier::LVNotifier(G4FaserGeometryNotifierSvc* gs)
  : m_notifierSvc(gs)
{
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  store->SetNotifier(this);
}

void LVNotifier::NotifyRegistration()
{
  G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  G4LogicalVolume* lV = store->back();
//   std::cout << "LVNotifier:  " << m_notifierSvc->GetCurrentDetectorName() + "::" + lV->GetName() << std::endl;
  lV->SetName( m_notifierSvc->GetCurrentDetectorName() + "::" + lV->GetName() );
}

void LVNotifier::NotifyDeRegistration()
{
}
