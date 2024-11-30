/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


//  Update: Riccardo Maria BIANCHI rbianchi@cern.ch Feb 2014                   //

#ifndef VTI12UTILS_HITTOSONODE_H
#define VTI12UTILS_HITTOSONODE_H

// This class is to allow the PRD and track systems to share the same code for drawing DEs.

#include "VP1Base/VP1HelperClassBase.h"

// Eigen Migration
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoPrimitives/GeoPrimitivesHelpers.h"
// new files
#include "GeoPrimitives/CLHEPtoEigenEulerAnglesConverters.h"
#include "GeoPrimitives/EulerAnglesHelpers.h"


class SoSeparator;
class SoTransform;

namespace Trk {
  class RIO_OnTrack;
  class TrkDetElementBase;
}
class Identifier;

class HitToSoNode : public VP1HelperClassBase {
public:

  HitToSoNode( IVP1System * sys = 0 );//sys for messages
  ~HitToSoNode();
  
  SoTransform * createTransform(const Trk::RIO_OnTrack& rio, bool useSurfacePositionOnly) const;
  void buildStripShapes(const Trk::RIO_OnTrack& rio, SoSeparator*&shape_simple, SoSeparator*&shape_detailed, bool blockGP);
  // void buildTubeShapes(const Trk::RIO_OnTrack& rio, SoSeparator*&shape_simple, SoSeparator*&shape_detailed, bool blockGP, double length, bool doProjection);
  
private:

  HitToSoNode( const HitToSoNode & );
  HitToSoNode & operator= ( const HitToSoNode & );

  // Eigen migration
//  void fillValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth,  double& stripThickness, Trk::LocalPosition*& localposStrip);
//  void fillRPCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,  Trk::LocalPosition*& localposStrip);
//  void fillTGCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,  Trk::LocalPosition*& localposStrip);
//  void fillCSCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,  Trk::LocalPosition*& localposStrip);
//  void fillSTGCValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness, Trk::LocalPosition*& localposStrip);
//  void fillMMValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,   Trk::LocalPosition*& localposStrip);
//  void fillSiValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,   Trk::LocalPosition*& localposStrip);
//
  void fillValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth,  double& stripThickness, Amg::Vector2D*& localposStrip);
  void fillSiValues(Identifier& id, const Trk::TrkDetElementBase* baseDetEl, double& striplength, double& stripWidth, double& stripThickness,   Amg::Vector2D*& localposStrip);

  class Imp;
  Imp * m_d;
};

#endif
