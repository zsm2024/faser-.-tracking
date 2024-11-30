/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERALG_G4FASERUSERWORKERTHREADINITIALIZATION_H
#define G4FASERALG_G4FASERUSERWORKERTHREADINITIALIZATION_H

// Hide multi-threading classes from builds without G4MT
#include "G4Types.hh"
#ifdef G4MULTITHREADED

#include "G4UserWorkerThreadInitialization.hh"

/// @brief ATLAS custom class for thread initialization functionality
///
/// What we currently override from the G4 base class:
/// * Thread-private random number generator engine
/// * Creation of a worker run manager (G4FaserWorkerRunManager)
///
/// TODO: do we need messaging?
/// TODO: make this work better with HepRndm Engine.
///       Currently seeds will not be set correctly!
///
/// @author Steve Farrell <Steven.Farrell@cern.ch>
///
class G4FaserUserWorkerThreadInitialization : public G4UserWorkerThreadInitialization {

public:

  /// @brief Create ATLAS worker run manager
  G4WorkerRunManager* CreateWorkerRunManager() const;

  /// @name Methods to pass configuration in from G4FaserAlg
  /// @{
  /// Configure the detector geometry service handle
  void SetDetGeoSvc(const std::string& typeAndName) {
    m_detGeoSvcName = typeAndName;
  }

  /// Configure the Fast Simulation Master Tool handle
  void SetFastSimMasterTool(const std::string& typeAndName) {
    m_fastSimToolName = typeAndName;
  }

private:
  std::string m_detGeoSvcName{"DetectorGeometrySvc"};
  std::string m_fastSimToolName{"FastSimulationMasterTool"};

}; // class G4FaserUserWorkerThreadInitialization

#endif // G4MULTITHREADED

#endif

