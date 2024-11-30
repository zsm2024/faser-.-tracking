/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDCollHandle_SpacePoints         //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: September 2008                           //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12PRDSystems/PRDCollHandle_SpacePoints.h"
#include "VTI12PRDSystems/PRDHandle_SpacePoint.h"
#include "VTI12PRDSystems/PRDSystemController.h"
#include "VP1Base/IVP1System.h"
#include "VTI12Utils/VP1JobConfigInfo.h"
#include "VTI12Utils/VP1SGContentsHelper.h"
#include "VTI12Utils/VP1SGAccessHelper.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include <QColor>

//____________________________________________________________________
QStringList PRDCollHandle_SpacePoints::availableCollections(IVP1System*sys)
{
  if (!VP1JobConfigInfo::hasSCTGeometry())
    return QStringList();
  QStringList keys = VP1SGContentsHelper(sys).getKeys<FaserSCT_SpacePointContainer>();
  QStringList unsafekeys;
  if (!VP1JobConfigInfo::hasSCTGeometry()) {
    foreach (QString key,keys) {
      if (key.contains("sct",Qt::CaseInsensitive))
	unsafekeys << key;
    }
  }
  foreach (QString unsafekey,unsafekeys)
    keys.removeAll(unsafekey);
  return keys;
}

//____________________________________________________________________
class PRDCollHandle_SpacePoints::Imp {
public:
  PRDCommonFlags::TrackerPartsFlags trackerpartsflags;
};


//____________________________________________________________________
PRDCollHandle_SpacePoints::PRDCollHandle_SpacePoints(PRDSysCommonData* common,const QString& key)
  : PRDCollHandleBase(PRDDetType::SpacePoints,common,key), m_d(new Imp)
{
  m_d->trackerpartsflags = (PRDCommonFlags::All);
}

//____________________________________________________________________
PRDCollHandle_SpacePoints::~PRDCollHandle_SpacePoints()
{
  delete m_d;
}

//____________________________________________________________________
QColor PRDCollHandle_SpacePoints::defaultColor() const
{
  return QColor::fromRgbF(1.0, 1.0, 0.5 );//light yellow
}

//____________________________________________________________________
bool PRDCollHandle_SpacePoints::load()
{
  /////////////////////////////////////////////////////////////////////////
  // Retrieve element container from event store:
  const FaserSCT_SpacePointContainer* container;
  if(!VP1SGAccessHelper(systemBase()).retrieve(container,text()))
    return false;

  int isp(0);
  FaserSCT_SpacePointContainer::const_iterator itElement, itElementE(container->end());
  for (itElement=container->begin();itElement!=itElementE;++itElement) {
    FaserSCT_SpacePointContainer::base_value_type::const_iterator it, itE((*itElement)->end());
    for (it=(*itElement)->begin();it!=itE;++it) {
      const Tracker::FaserSCT_SpacePoint * sp = *it;
      ++isp;
      if (!sp) {
	message("WARNING - ignoring null spacepoint pointer.");
	continue;
      }
      if (!sp->clusterList().first) {
	message("WARNING - ignoring spacepoint with null first cluster.");
	continue;
      }
      PRDHandleBase * handle = new PRDHandle_SpacePoint(this,sp);
      if (handle) {
	addHandle(handle);
	//We just register the first of the (possibly) two prds here (fixme: check that it works):
	common()->registerPRD2Handle(handle->getPRD(),handle);
      } else {
	message("WARNING - ignoring null handle pointer.");
      }
      if (!(isp%100))
	systemBase()->updateGUI();
    }
  }
  return true;
}

//____________________________________________________________________
bool PRDCollHandle_SpacePoints::cut(PRDHandleBase*handlebase)
{
  PRDHandle_SpacePoint * handle = static_cast<PRDHandle_SpacePoint*>(handlebase);
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

  return true;
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::eraseEventDataSpecific()
{
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::postLoadInitialisation()
{
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::setupSettingsFromControllerSpecific(PRDSystemController*controller)
{
  connect(controller,SIGNAL(trackerPartsFlagsChanged(PRDCommonFlags::TrackerPartsFlags)),
	  this,SLOT(setPartsFlags(PRDCommonFlags::TrackerPartsFlags)));
  setPartsFlags(controller->trackerPartsFlags());
}

//____________________________________________________________________
void PRDCollHandle_SpacePoints::setPartsFlags(PRDCommonFlags::TrackerPartsFlags flags)
{
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
    PRDHandle_SpacePoint* handle = static_cast<PRDHandle_SpacePoint*>(*it);
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
