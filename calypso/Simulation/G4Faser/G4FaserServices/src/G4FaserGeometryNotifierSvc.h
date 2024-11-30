/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERSERVICES_G4FASERGEOMETRYNOTIFIERSVC_H
#define G4FASERSERVICES_G4FASERGEOMETRYNOTIFIERSVC_H

// Base classes
#include "AthenaBaseComps/AthService.h"
#include "G4AtlasInterfaces/IG4GeometryNotifierSvc.h"

class G4VNotifier;

class G4FaserGeometryNotifierSvc : public extends<AthService, IG4GeometryNotifierSvc> {
public:
  // Standard constructor and destructor
  G4FaserGeometryNotifierSvc( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~G4FaserGeometryNotifierSvc();

  // Gaudi methods
  StatusCode initialize() override final;
  StatusCode finalize() override final;

  //FIXME This is a bit nasty as it is not thread-safe, but we assume
  //that the geometry building will be done in a single thread.
  void SetCurrentDetectorName(const std::string& s) override final {m_currentDetectorName=s;}
  const std::string GetCurrentDetectorName() const override final {return m_currentDetectorName;}

private:
  std::string m_currentDetectorName{""};

  Gaudi::Property<bool> m_activateLVNotifier{this, "ActivateLVNotifier", true, "Toggle on/off the G4 LV notifier"};
  Gaudi::Property<bool> m_activatePVNotifier{this, "ActivatePVNotifier", false, "Toggle on/off the G4 PV notifier"};
  G4VNotifier* lvNotifier{};
  G4VNotifier* pvNotifier{};
};

#endif //G4FASERSERVICES_G4FASERGEOMETRYNOTIFIERSVC_H

