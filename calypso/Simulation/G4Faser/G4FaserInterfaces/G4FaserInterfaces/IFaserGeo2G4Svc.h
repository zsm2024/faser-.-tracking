/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FaserInterfaces_IFaserGeo2G4Svc_H
#define G4FaserInterfaces_IFaserGeo2G4Svc_H

#include "GaudiKernel/IService.h"
#include "FaserGeo2G4SvcBase.h"

class IFaserGeo2G4Svc: virtual public IService,
                  virtual public FaserGeo2G4SvcBase {
public:
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IFaserGeo2G4Svc, 1, 0);
};
#endif // G4FaserInterfaces_IFaserGeo2G4Svc_H

