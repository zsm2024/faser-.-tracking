/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELINTERFACES_IGEODBTAGSVC_H
#define GEOMODELINTERFACES_IGEODBTAGSVC_H

// Include Files
#include "GaudiKernel/IInterface.h"
#include <string>

namespace GeoModel {
  enum GeoConfig {
    GEO_RUN1,
    GEO_RUN2,
    GEO_RUN3,
    GEO_RUN4,
    GEO_ITk,
    GEO_TESTBEAM
  };
}  

static const InterfaceID IID_IGeoDbTagSvc("IGeoDbTagSvc", 1, 0);

class IGeoDbTagSvc : virtual public IInterface {
 public:
  // Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IGeoDbTagSvc;}

  virtual const std::string & faserVersion()                    const =0;
  virtual const std::string & neutrinoVersion()                 const =0;
  virtual const std::string & emulsionVersion()                 const =0;
  virtual const std::string & scintVersion()                    const =0;
  virtual const std::string & vetoVersion()                     const =0; 
  virtual const std::string & vetoNuVersion()                   const =0; 
  virtual const std::string & triggerVersion()                  const =0; 
  virtual const std::string & preshowerVersion()                const =0; 
  virtual const std::string & trackerVersion()                  const =0;
  virtual const std::string & sctVersion()                      const =0;
  virtual const std::string & dipoleVersion()                   const =0;
  virtual const std::string & caloVersion()                     const =0;
  virtual const std::string & ecalVersion()                     const =0;
  // virtual const std::string & magFieldVersion()                 const =0;
  virtual const std::string & trenchVersion()                   const =0;
  
  virtual const std::string & neutrinoVersionOverride()         const =0;
  virtual const std::string & emulsionVersionOverride()         const =0;
  virtual const std::string & scintVersionOverride()            const =0;
  virtual const std::string & vetoVersionOverride()             const =0;
  virtual const std::string & vetoNuVersionOverride()           const =0;
  virtual const std::string & triggerVersionOverride()          const =0;
  virtual const std::string & preshowerVersionOverride()        const =0;
  virtual const std::string & trackerVersionOverride()          const =0;
  virtual const std::string & sctVersionOverride()              const =0;
  virtual const std::string & dipoleVersionOverride()           const =0;
  virtual const std::string & caloVersionOverride()             const =0;
  virtual const std::string & ecalVersionOverride()             const =0;
  // virtual const std::string & magFieldVersionOverride()         const =0;
  virtual const std::string & trenchVersionOverride()           const =0;
  
  virtual GeoModel::GeoConfig geoConfig() const = 0;
};

#endif // GEOMODELINTERFACES_IGEODBTAGSVC_H



