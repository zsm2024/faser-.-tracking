/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_IGEOMODELSVC_H
#define GEOMODELINTERFACES_IGEOMODELSVC_H

// Include Files
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include <string>

// Forward declarations
class IIncidentListener;
class Incident;
class IGeoModelTool;

class IGeoModelSvc : virtual public IInterface {

public:

    /// Retrieve interface ID
  static const InterfaceID& interfaceID();


    virtual const std::string & faserVersion()         const =0;
    virtual const std::string & neutrinoVersion()      const =0;
    virtual const std::string & emulsionVersion()      const =0;
    virtual const std::string & scintVersion()         const =0;
    virtual const std::string & vetoVersion()          const =0;
    virtual const std::string & vetoNuVersion()          const =0;
    virtual const std::string & triggerVersion()       const =0;
    virtual const std::string & preshowerVersion()     const =0;
    virtual const std::string & trackerVersion()       const =0;
    virtual const std::string & sctVersion()           const =0;
    virtual const std::string & dipoleVersion()        const =0;
    virtual const std::string & caloVersion()          const =0;
    virtual const std::string & ecalVersion()          const =0;
    // virtual const std::string & magFieldVersion()      const =0;
    virtual const std::string & trenchVersion()        const =0;

    virtual const std::string & neutrinoVersionOverride()  const =0;
    virtual const std::string & emulsionVersionOverride()  const =0;
    virtual const std::string & scintVersionOverride()     const =0;
    virtual const std::string & vetoVersionOverride()      const =0;
    virtual const std::string & vetoNuVersionOverride()      const =0;
    virtual const std::string & triggerVersionOverride()   const =0;
    virtual const std::string & preshowerVersionOverride() const =0;
    virtual const std::string & trackerVersionOverride()   const =0;
    virtual const std::string & dipoleVersionOverride()    const =0;
    virtual const std::string & sctVersionOverride()       const =0;
    virtual const std::string & caloVersionOverride()      const =0;
    virtual const std::string & ecalVersionOverride()      const =0;
    // virtual const std::string & magFieldVersionOverride()    const =0;
    virtual const std::string & trenchVersionOverride() const =0;
    
    virtual GeoModel::GeoConfig geoConfig() const = 0;

    // Access to subsystem tool for callback registration
    virtual const IGeoModelTool* getTool(std::string toolName) const = 0;

    // Release GeoModel tree from memory
    virtual StatusCode clear() = 0;
};

inline
const InterfaceID&
IGeoModelSvc::interfaceID() {

  static const InterfaceID IID(1011, 1, 1);

  return IID;
}


#endif // GEOMODELINTERFACES_GEOMODELSVC_H



