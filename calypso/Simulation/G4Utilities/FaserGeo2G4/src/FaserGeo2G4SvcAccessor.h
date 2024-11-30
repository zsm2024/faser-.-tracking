/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4SvcAccessor_H
#define FASERGEO2G4_FaserGeo2G4SvcAccessor_H

#include "G4FaserInterfaces/FaserGeo2G4SvcBase.h"
#include "G4FaserInterfaces/IFaserGeo2G4Svc.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

class FaserGeo2G4SvcAccessor {
public:
  FaserGeo2G4SvcBase* GetGeo2G4Svc() const;
};

inline FaserGeo2G4SvcBase* FaserGeo2G4SvcAccessor::GetGeo2G4Svc() const
{
  IService* g2gSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
  StatusCode result = svcLocator->service("FaserGeo2G4Svc",g2gSvc);

  if(result.isSuccess())
    {
      FaserGeo2G4SvcBase* temp=dynamic_cast<FaserGeo2G4SvcBase*>(g2gSvc);
      return temp;
    }
  else
    return 0;
}

#endif
