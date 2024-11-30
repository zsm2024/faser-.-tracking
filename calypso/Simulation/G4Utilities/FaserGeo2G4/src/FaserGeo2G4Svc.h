/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeo2G4Svc_H
#define FASERGEO2G4_FaserGeo2G4Svc_H

#include "G4FaserInterfaces/IFaserGeo2G4Svc.h"

#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IIncidentListener.h"

#include "FaserGeo2G4AssemblyFactory.h"

#include "FaserVolumeBuilder.h"

#include <string>
#include <map>
#include <memory>

typedef std::unordered_map< std::string, std::unique_ptr<FaserVolumeBuilder> > BuilderMap;

/// @todo NEEDS DOCUMENTATION
class FaserGeo2G4Svc: public extends<AthService, IFaserGeo2G4Svc, IIncidentListener>
{
public:
  FaserGeo2G4Svc(const std::string& , ISvcLocator *);
  virtual ~FaserGeo2G4Svc();
  /// AthService methods
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;
  /// IIncidentListener methods -  FIXME does this service actually need to listen for Incidents?
  virtual void handle(const Incident&) override final;
  /// Geo2G4SvcBase methods
  // virtual void RegisterVolumeBuilder(VolumeBuilder* vb) override final;
  // virtual void UnregisterVolumeBuilder(VolumeBuilder* vb) override final;
  // virtual void SetDefaultBuilder(VolumeBuilder *vb) override final {m_defaultBuilder=vb;}
  virtual void SetDefaultBuilder(std::string n) override final { m_defaultBuilder = n; }
  virtual FaserVolumeBuilder* GetVolumeBuilder(std::string s) const override final;
  virtual FaserVolumeBuilder* GetDefaultBuilder() const override final {return m_builders.at(m_defaultBuilder).get();}
  virtual bool UseTopTransforms() const override final {return m_getTopTransform;}
  virtual void ListVolumeBuilders() const override final;
private:
  std::string m_defaultBuilder;
  BuilderMap m_builders ;
  bool m_getTopTransform;
  std::unique_ptr<FaserGeo2G4AssemblyFactory> m_G4AssemblyFactory;
};

#endif
