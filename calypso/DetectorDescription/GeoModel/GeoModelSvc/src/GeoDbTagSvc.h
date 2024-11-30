/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef GEOMODELSVC_GEODBTAGSVC_H
#define GEOMODELSVC_GEODBTAGSVC_H

#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "AthenaBaseComps/AthService.h"

template <class TYPE> class SvcFactory;

class GeoDbTagSvc : public AthService, virtual public IGeoDbTagSvc
{
  friend class GeoModelSvc;

 public:
  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  friend class SvcFactory<GeoDbTagSvc>;

  GeoDbTagSvc(const std::string& name, ISvcLocator* svc);
  virtual ~GeoDbTagSvc();

 protected:

  void setFaserVersion(const std::string& tag)                     { m_FaserVersion=tag; }
  void setNeutrinoVersionOverride(const std::string& tag)          { m_NeutrinoVersionOverride=tag; }
  void setEmulsionVersionOverride(const std::string& tag)          { m_EmulsionVersionOverride=tag; }
  void setScintVersionOverride(const std::string& tag)             { m_ScintVersionOverride=tag; }
  void setVetoVersionOverride(const std::string& tag)              { m_VetoVersionOverride=tag; }
  void setVetoNuVersionOverride(const std::string& tag)            { m_VetoNuVersionOverride=tag; }
  void setTriggerVersionOverride(const std::string& tag)           { m_TriggerVersionOverride=tag; }
  void setPreshowerVersionOverride(const std::string& tag)         { m_PreshowerVersionOverride=tag; }
  void setTrackerVersionOverride(const std::string& tag)           { m_TrackerVersionOverride=tag; }
  void setSCTVersionOverride(const std::string& tag)               { m_SCTVersionOverride=tag; }
  void setDipoleVersionOverride(const std::string& tag)            { m_DipoleVersionOverride=tag; }
  void setCaloVersionOverride(const std::string& tag)              { m_CaloVersionOverride=tag; }
  void setEcalVersionOverride(const std::string& tag)              { m_EcalVersionOverride=tag; }
  // void setMagFieldVersionOverride(const std::string& tag)          { m_MagFieldVersionOverride=tag; }
  void setTrenchVersionOverride(const std::string& tag)            { m_TrenchVersionOverride=tag; }

  StatusCode setupTags();

 private:
  // ______________________________ IGeoDbTagSvc ____________________________________
  const std::string & faserVersion()                     const { return m_FaserVersion; }
  const std::string & neutrinoVersionOverride()          const { return m_NeutrinoVersionOverride; }
  const std::string & emulsionVersionOverride()          const { return m_EmulsionVersionOverride; }
  const std::string & scintVersionOverride()             const { return m_ScintVersionOverride; }
  const std::string & vetoVersionOverride()              const { return m_VetoVersionOverride; }
  const std::string & vetoNuVersionOverride()            const { return m_VetoNuVersionOverride; }
  const std::string & triggerVersionOverride()           const { return m_TriggerVersionOverride; }
  const std::string & preshowerVersionOverride()         const { return m_PreshowerVersionOverride; }
  const std::string & trackerVersionOverride()           const { return m_TrackerVersionOverride; }
  const std::string & sctVersionOverride()               const { return m_SCTVersionOverride; }
  const std::string & dipoleVersionOverride()            const { return m_DipoleVersionOverride; }
  const std::string & caloVersionOverride()              const { return m_CaloVersionOverride; }
  const std::string & ecalVersionOverride()              const { return m_EcalVersionOverride; }
  // const std::string & magFieldVersionOverride()          const { return m_MagFieldVersionOverride; }
  const std::string & trenchVersionOverride()            const { return m_TrenchVersionOverride; }

  const std::string & neutrinoVersion()                  const { return m_NeutrinoVersion; }
  const std::string & emulsionVersion()                  const { return m_EmulsionVersion; }
  const std::string & scintVersion()                     const { return m_ScintVersion; }
  const std::string & vetoVersion()                      const { return m_VetoVersion; }
  const std::string & vetoNuVersion()                    const { return m_VetoNuVersion; }
  const std::string & triggerVersion()                   const { return m_TriggerVersion; }
  const std::string & preshowerVersion()                 const { return m_PreshowerVersion; }
  const std::string & trackerVersion()                   const { return m_TrackerVersion; }
  const std::string & sctVersion()                       const { return m_SCTVersion; }
  const std::string & dipoleVersion()                    const { return m_DipoleVersion; }
  const std::string & caloVersion()                      const { return m_CaloVersion; }
  const std::string & ecalVersion()                      const { return m_EcalVersion; }
  // const std::string & magFieldVersion()                  const { return m_MagFieldVersion; }
  const std::string & trenchVersion()               const { return m_TrenchVersion; }

  GeoModel::GeoConfig geoConfig() const { return m_geoConfig; }

  // _________________________ Private data Members _______________________________
  std::string m_FaserVersion;

  std::string m_NeutrinoVersion;
  std::string m_EmulsionVersion;
  std::string m_ScintVersion;
  std::string m_VetoVersion;
  std::string m_VetoNuVersion;
  std::string m_TriggerVersion;
  std::string m_PreshowerVersion;
  std::string m_TrackerVersion;
  std::string m_SCTVersion;
  std::string m_DipoleVersion;
  std::string m_CaloVersion;
  std::string m_EcalVersion;
  // std::string m_MagFieldVersion;
  std::string m_TrenchVersion;

  std::string m_NeutrinoVersionOverride;
  std::string m_EmulsionVersionOverride;
  std::string m_ScintVersionOverride;
  std::string m_VetoVersionOverride;
  std::string m_VetoNuVersionOverride;
  std::string m_TriggerVersionOverride;
  std::string m_PreshowerVersionOverride;
  std::string m_TrackerVersionOverride;
  std::string m_SCTVersionOverride;
  std::string m_DipoleVersionOverride;
  std::string m_CaloVersionOverride;
  std::string m_EcalVersionOverride;
  // std::string m_MagFieldVersionOverride;
  std::string m_TrenchVersionOverride;

  GeoModel::GeoConfig m_geoConfig;
};

#endif // GEOMODELSVC_GEODBTAGSVC_H
