/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FaserGeometryNotifierSvc.h"

#include "PVNotifier.h"
#include "LVNotifier.h"

G4FaserGeometryNotifierSvc::G4FaserGeometryNotifierSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class(name,pSvcLocator)
{
  ATH_MSG_DEBUG( "G4FaserGeometryNotifierSvc being created!" );
}

G4FaserGeometryNotifierSvc::~G4FaserGeometryNotifierSvc()
{
  if (lvNotifier) delete lvNotifier;
  if (pvNotifier) delete pvNotifier;
}

StatusCode G4FaserGeometryNotifierSvc::initialize(){
  if (m_activateLVNotifier || m_activatePVNotifier)
    {
      ATH_MSG_DEBUG( "Notifiers being instantiated " );
      if (m_activateLVNotifier) lvNotifier=new LVNotifier(this);
      if (m_activatePVNotifier) pvNotifier=new PVNotifier(this);
    }

  ATH_MSG_DEBUG( "G4FaserGeometryNotifierSvc initialized!!!" );
  return StatusCode::SUCCESS;
}

StatusCode G4FaserGeometryNotifierSvc::finalize(){
  ATH_MSG_DEBUG( "G4FaserGeometryNotifierSvc being finalized!!!" );
  return StatusCode::SUCCESS;
}

