/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PVNotifier_H
#define PVNotifier_H

#include "G4VNotifier.hh"

class G4FaserGeometryNotifierSvc;

/// @class PVNotifier
/// Notifier class to prepend detector name to all G4 Physical Volumes
/// Only to be used by the G4GeometryNotifierSvc
class PVNotifier :  public G4VNotifier
{
  friend class G4FaserGeometryNotifierSvc;

  public:

    void NotifyRegistration();
    void NotifyDeRegistration();

  private:

    PVNotifier(G4FaserGeometryNotifierSvc*);

    G4FaserGeometryNotifierSvc* m_notifierSvc;
};

#endif

