/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4FASERALG_G4FASERACTIONINITIALIZATION_H
#define G4FASERALG_G4FASERACTIONINITIALIZATION_H 1

#include "G4VUserActionInitialization.hh"
#include "G4AtlasInterfaces/IUserActionSvc.h"

/// Action initialization class.
///

class G4FaserActionInitialization : public G4VUserActionInitialization
{
public:
  G4FaserActionInitialization(G4UA::IUserActionSvc* userActionSvc);
  virtual ~G4FaserActionInitialization();

  virtual void BuildForMaster() const;
  virtual void Build() const;

private:
  G4UA::IUserActionSvc* m_userActionSvc{};
};

#endif //G4FASERALG_G4FASERACTIONINITIALIZATION_H

