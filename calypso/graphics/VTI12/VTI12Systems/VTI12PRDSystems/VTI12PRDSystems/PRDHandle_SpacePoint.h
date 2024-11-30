/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class PRDHandle_SpacePoint                //
//                                                            //
//  Description: Handles for SpacePoints (not really "PRDs")  //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDHANDLE_SPACEPOINT_H
#define PRDHANDLE_SPACEPOINT_H

#include "VTI12PRDSystems/PRDHandleBase.h"

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrkPrepRawData/PrepRawData.h"

#include "GeoPrimitives/GeoPrimitives.h"



// namespace Trk { class SpacePoint; }
class PRDCollHandle_SpacePoints;

class PRDHandle_SpacePoint : public PRDHandleBase {
public:

  PRDHandle_SpacePoint(PRDCollHandle_SpacePoints*, const Tracker::FaserSCT_SpacePoint*);
  virtual ~PRDHandle_SpacePoint();

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  Amg::Vector3D center() const { return m_sp->globalPosition(); }

//SCT spacepoints have two clusters, Pixels one:
  bool isSCT() { return getSecondPRD(); }

  const Tracker::FaserSCT_SpacePoint * spacePoint() const { return m_sp; }
  const Trk::PrepRawData * getPRD() const { return m_sp->clusterList().first; }
  const Trk::PrepRawData * getSecondPRD() const { return m_sp->clusterList().second; }
  bool isInterface()  const { return dynamic_cast<const TrackerDD::SiDetectorElement*>(getPRD()->detectorElement())->isInterface(); }
  bool isUpstream()   const { return dynamic_cast<const TrackerDD::SiDetectorElement*>(getPRD()->detectorElement())->isUpstream(); }
  bool isCentral()    const { return dynamic_cast<const TrackerDD::SiDetectorElement*>(getPRD()->detectorElement())->isCentral(); }
  bool isDownstream() const { return dynamic_cast<const TrackerDD::SiDetectorElement*>(getPRD()->detectorElement())->isDownstream(); }
  // bool isBarrel() const
  // { return static_cast<const InDetDD::SiDetectorElement*>(getPRD()->detectorElement())->isBarrel(); }
  // bool isPositiveZ() const { return m_sp->globalPosition().z() > 0.0; }

private:

  // Uncomment to make it illegal to copy/assign a PRDHandle_SpacePoint:
  // PRDHandle_SpacePoint( const PRDHandle_SpacePoint & );
  // PRDHandle_SpacePoint & operator= ( const PRDHandle_SpacePoint & );

  const Tracker::FaserSCT_SpacePoint* m_sp;
  class Imp;
  Imp * m_d;

};

#endif
