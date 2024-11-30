/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LVNotifier_H
#define LVNotifier_H

#include "G4VNotifier.hh"

class G4FaserGeometryNotifierSvc;

/// @class LVNotifier
/// @todo NEEDS DOCUMENTATION
class LVNotifier : public G4VNotifier
{
  friend class G4FaserGeometryNotifierSvc;

  public:

    void NotifyRegistration();
    void NotifyDeRegistration();

  private:

    LVNotifier(G4FaserGeometryNotifierSvc*);

    G4FaserGeometryNotifierSvc* m_notifierSvc;
};

#endif

