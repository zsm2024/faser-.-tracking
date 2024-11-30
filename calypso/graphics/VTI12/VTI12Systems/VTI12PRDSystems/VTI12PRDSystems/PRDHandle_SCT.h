/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/

#ifndef PRDHANDLE_SCT_H
#define PRDHANDLE_SCT_H

#include "VTI12PRDSystems/PRDHandleBase.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"

class PRDCollHandle_SCT;

class PRDHandle_SCT : public PRDHandleBase {
public:

  PRDHandle_SCT(PRDCollHandle_SCT*,const Tracker::FaserSCT_Cluster*);
  virtual ~PRDHandle_SCT() {};

  void buildShapes(SoNode*&shape_simple, SoNode*&shape_detailed);
  int regionIndex();

  const Tracker::FaserSCT_Cluster * cluster() const { return m_cluster; }
  const Trk::PrepRawData * getPRD() const { return m_cluster; }
  bool isInterface()  const { return m_cluster->detectorElement()->isInterface(); }
  bool isUpstream()   const { return m_cluster->detectorElement()->isUpstream(); }
  bool isCentral()    const { return m_cluster->detectorElement()->isCentral(); }
  bool isDownstream() const { return m_cluster->detectorElement()->isDownstream(); }

private:
  const Tracker::FaserSCT_Cluster* m_cluster;

};

#endif
