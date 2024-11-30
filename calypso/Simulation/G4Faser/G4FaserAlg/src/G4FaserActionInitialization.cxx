/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "G4FaserAlg/G4FaserActionInitialization.h"
#if G4VERSION_NUMBER >= 1070
#include "G4Exception.hh"
#else
#include "globals.hh"
#endif

G4FaserActionInitialization::G4FaserActionInitialization(G4UA::IUserActionSvc* userActionSvc)
  : G4VUserActionInitialization()
  , m_userActionSvc(userActionSvc)
{}


G4FaserActionInitialization::~G4FaserActionInitialization()
{}


void G4FaserActionInitialization::BuildForMaster() const
{
  G4VUserActionInitialization::BuildForMaster();
  // TODO Ideally separate "master" versions of the G4RunActions would
  // be created, but this will require a larger redesign. See
  // https://gitlab.cern.ch/geant4/geant4/-/blob/master/examples/basic/B4/B4a/src/B4aActionInitialization.cc#L52
}


void G4FaserActionInitialization::Build() const
{
  if (!m_userActionSvc) {
    G4ExceptionDescription description;
    description << "Build: UserActionSvc is NULL.";
    G4Exception("G4FaserActionInitialization", "NoUserActionSvc", FatalException, description);
    abort(); // to keep Coverity happy
  }
  if (m_userActionSvc->initializeActions().isFailure()) { //Consider renaming to buildActions()?
    G4ExceptionDescription description;
    description << "Build: Failed to create UserActions.";
    G4Exception("G4FaserActionInitialization", "CouldNotBuildActions", FatalException, description);
    abort(); // to keep Coverity happy
  }
}

