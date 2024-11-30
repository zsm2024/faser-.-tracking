/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VTI12PRDSystems/PRDCollHandle_SCT.h"
#include "VTI12PRDSystems/PRDHandle_SCT.h"
#include "VTI12PRDSystems/PRDSystemController.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include <QColor>

#include "VTI12Utils/VP1JobConfigInfo.h"
#include "VTI12Utils/VP1SGContentsHelper.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"

//____________________________________________________________________
QStringList PRDCollHandle_SCT::availableCollections(IVP1System*sys)
{
  return VP1JobConfigInfo::hasSCTGeometry() ? VP1SGContentsHelper(sys).getKeys<Tracker::FaserSCT_ClusterContainer>() : QStringList();
}

//____________________________________________________________________
class PRDCollHandle_SCT::Imp {
public:
  std::set<const TrackerDD::SiDetectorElement*> touchedelements;

  PRDCommonFlags::TrackerPartsFlags trackerpartsflags;
  bool excludeisolatedclusters = false;
};

//____________________________________________________________________
PRDCollHandle_SCT::PRDCollHandle_SCT(PRDSysCommonData * cd,const QString& key)
  : PRDCollHandleBase(PRDDetType::SCT,cd,key), m_d(new Imp)
{
  m_d->trackerpartsflags = PRDCommonFlags::All;
  m_d->excludeisolatedclusters = true;
}

//____________________________________________________________________
PRDCollHandle_SCT::~PRDCollHandle_SCT()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_SCT::defaultColor() const
{
  return QColor::fromRgbF(1.0, 1.0, 0.5 );//light yellow
}

//____________________________________________________________________
PRDHandleBase* PRDCollHandle_SCT::addPRD( const Trk::PrepRawData * prd )
{
  assert(dynamic_cast<const Tracker::FaserSCT_Cluster*>(prd));
  return new PRDHandle_SCT(this,static_cast<const Tracker::FaserSCT_Cluster*>(prd));
}

//____________________________________________________________________
void PRDCollHandle_SCT::postLoadInitialisation()
{
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it)
    m_d->touchedelements.insert(static_cast<PRDHandle_SCT*>(*it)->cluster()->detectorElement());
  m_d->touchedelements.insert(0);//To always show clusters whose elements have no otherSide() pointer.
}

//____________________________________________________________________
void PRDCollHandle_SCT::eraseEventDataSpecific()
{
  m_d->touchedelements.clear();
}

//____________________________________________________________________
bool PRDCollHandle_SCT::cut(PRDHandleBase*handlebase)
{
  PRDHandle_SCT * handle = static_cast<PRDHandle_SCT*>(handlebase);
  assert(handle);

  if (m_d->trackerpartsflags!=PRDCommonFlags::All) {
    if (handle->isInterface()) 
        if (!(m_d->trackerpartsflags & PRDCommonFlags::Interface)) return false;
    if (handle->isUpstream())
        if (!(m_d->trackerpartsflags & PRDCommonFlags::Upstream)) return false;
    if (handle->isCentral())
        if (!(m_d->trackerpartsflags & PRDCommonFlags::Central)) return false;
    if (handle->isDownstream())
        if (!(m_d->trackerpartsflags & PRDCommonFlags::Downstream)) return false;        
  }

  if (m_d->excludeisolatedclusters) {
    if (!m_d->touchedelements.count(handle->cluster()->detectorElement()->otherSide()))
      return false;
  }

  return true;
}

//____________________________________________________________________
void PRDCollHandle_SCT::setPartsFlags(PRDCommonFlags::TrackerPartsFlags flags ) {
  //NB: The code is this method is very similar in PRDCollHandle_Pixel::setPartsFlags, PRDCollHandle_SCT::setPartsFlags,
  //PRDCollHandle_TRT::setPartsFlags and and PRDCollHandle_SpacePoints::setPartsFlags
  //Fixme: base decision to recheck on visibility also!

  if (m_d->trackerpartsflags==flags)
    return;

  bool interfaceChanged = (m_d->trackerpartsflags&PRDCommonFlags::Interface)!=(flags&PRDCommonFlags::Interface);
  bool upstreamChanged = (m_d->trackerpartsflags&PRDCommonFlags::Upstream)!=(flags&PRDCommonFlags::Upstream);
  bool centralChanged = (m_d->trackerpartsflags&PRDCommonFlags::Central)!=(flags&PRDCommonFlags::Central);
  bool downstreamChanged = (m_d->trackerpartsflags&PRDCommonFlags::Downstream)!=(flags&PRDCommonFlags::Downstream);

  m_d->trackerpartsflags=flags;

  largeChangesBegin();
  std::vector<PRDHandleBase*>::iterator it(getPrdHandles().begin()),itE(getPrdHandles().end());
  for (;it!=itE;++it) {
    PRDHandle_SCT* handle = static_cast<PRDHandle_SCT*>(*it);
    if (handle->isInterface()) 
    {
        if (interfaceChanged) recheckCutStatus(handle);
    }
    else if (handle->isUpstream())
    {
        if (upstreamChanged) recheckCutStatus(handle);
    }
    else if (handle->isCentral())
    {
        if (centralChanged) recheckCutStatus(handle);
    }
    else if (handle->isDownstream())
    {
        if (downstreamChanged) recheckCutStatus(handle);
    }
  }
  largeChangesEnd();

}

//____________________________________________________________________
void PRDCollHandle_SCT::setExcludeIsolatedClusters(bool excludeisolated)
{
  if (m_d->excludeisolatedclusters==excludeisolated)
    return;
  m_d->excludeisolatedclusters=excludeisolated;
  if (excludeisolated)
    recheckCutStatusOfAllVisibleHandles();
  else
    recheckCutStatusOfAllNotVisibleHandles();
}

//____________________________________________________________________
void PRDCollHandle_SCT::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(trackerPartsFlagsChanged(PRDCommonFlags::TrackerPartsFlags)),this,SLOT(setPartsFlags(PRDCommonFlags::TrackerPartsFlags)));
  setPartsFlags(controller->trackerPartsFlags());

  connect(controller,SIGNAL(sctExcludeIsolatedClustersChanged(bool)),this,SLOT(setExcludeIsolatedClusters(bool)));
  setExcludeIsolatedClusters(controller->sctExcludeIsolatedClusters());
}
