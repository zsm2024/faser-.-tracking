/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Implementation of class PhiSectorManager                           //
//                                                                     //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>              //
//                                                                     //
//  Initial VP1 version: September 2007                                //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VTI12GeometrySystems/PhiSectorManager.h"
#include "VP1Base/PhiSectionWidget.h"
#include "VP1Base/VP1ExtraSepLayerHelper.h"
#include "VP1Base/VP1Msg.h"
#include "VP1Base/IVP1System.h"
#include "VTI12Utils/VP1LinAlgUtils.h"

#include <VP1HEPVis/nodes/SoTubs.h>
#include "VP1HEPVis/nodes/SoPcons.h"
#include "VP1HEPVis/nodes/SoCons.h"
#include "VP1HEPVis/nodes/SoLAr.h"

#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

#include <Inventor/SbLinear.h>
#include <Inventor/SbViewportRegion.h>

#include <map>
#include <set>
#include <cmath>

#define NPHISECTORS 36

//____________________________________________________________________
class PhiSectorManager::Imp {
public:
  PhiSectionWidget * phisectionwidget;
  IVP1System * system;

  std::map<VP1GeoFlags::SubSystemFlag,SoSeparator *> subsysflag2sep;
  std::map<VP1GeoFlags::SubSystemFlag,SoSeparator *> subsysflag2labelsep;
  std::map<VP1GeoFlags::SubSystemFlag,std::map<int,VP1ExtraSepLayerHelper*> > subsysflag_2_iphi2sephelper;
  std::map<VP1GeoFlags::SubSystemFlag,std::map<int,VP1ExtraSepLayerHelper*> > subsysflag_2_iphi2labelsephelper;
  std::map<VP1GeoFlags::SubSystemFlag,VP1ExtraSepLayerHelper*> subsysflag_2_volAroundZSepHelper;

  //NB: If in the future we implement the phi-sector switching on a per-subsystem basis, just keep track of which switches are from which subsystems:
  std::map<int,std::set<SoSwitch*> > iphi2switches;
  std::map<int,std::set<SoSwitch*> > iphi2Labelswitches;

  //The rest of the stuff is for the dynamical "slicing" of volumes around the Z-axis:
  std::map<SoSwitch*,std::pair<SoTubs*,SoSeparator*> > volaroundZ_tubs;
  std::map<SoSwitch*,std::pair<SoPcons*,SoSeparator*> > volaroundZ_pcons;
  std::map<SoSwitch*,std::pair<SoCons*,SoSeparator*> > volaroundZ_cons;
  std::map<SoSwitch*,std::pair<SoLAr*,SoSeparator*> > volaroundZ_lar;
  std::map<SoSwitch*,std::pair<double,bool> > volaroundZ_switch2transfinfo;//switch -> phi rotation around z and rotated around y flag.
  template <class T>
  void updateRepresentationsOfVolsAroundZAxis_Specific(std::map<SoSwitch*,std::pair<T*,SoSeparator*> >& volaroundZ);
  void getPhiExtentOfShape( const SoTubs* tubs, double&  phistart, double& phispan);
  void getPhiExtentOfShape( const SoPcons* pcons, double&  phistart, double& phispan);
  void getPhiExtentOfShape( const SoCons* cons, double&  phistart, double& phispan);
  void getPhiExtentOfShape( const SoLAr* lar, double&  phistart, double& phispan);
  void copyShapeFieldsAndOverridePhiRange( const SoTubs* source, SoTubs* dest, double phistart, double phispan);
  void copyShapeFieldsAndOverridePhiRange( const SoPcons* source, SoPcons* dest, double phistart, double phispan);
  void copyShapeFieldsAndOverridePhiRange( const SoCons* source, SoCons* dest, double phistart, double phispan);
  void copyShapeFieldsAndOverridePhiRange( const SoLAr* source, SoLAr* dest, double phistart, double phispan);
  int nactivelargechanges;

  QVector<bool> currentlyEnabledPhiSectors;
};

//____________________________________________________________________
void PhiSectorManager::registerSubSystemSeparator(VP1GeoFlags::SubSystemFlag flag,SoSeparator * subsystemsep)
{
  assert(m_d->subsysflag2sep.find(flag)==m_d->subsysflag2sep.end());
  assert(m_d->subsysflag_2_iphi2sephelper.find(flag)==m_d->subsysflag_2_iphi2sephelper.end());
  m_d->subsysflag_2_iphi2sephelper[flag] = std::map<int,VP1ExtraSepLayerHelper*>();
  m_d->subsysflag2sep[flag] = subsystemsep;
  
  assert(m_d->subsysflag2labelsep.find(flag)==m_d->subsysflag2labelsep.end());
  assert(m_d->subsysflag_2_iphi2labelsephelper.find(flag)==m_d->subsysflag_2_iphi2labelsephelper.end());
  m_d->subsysflag_2_iphi2labelsephelper[flag] = std::map<int,VP1ExtraSepLayerHelper*>();
  m_d->subsysflag2labelsep[flag] = subsystemsep;
}

//____________________________________________________________________
VP1ExtraSepLayerHelper * PhiSectorManager::getSepHelperForNode(VP1GeoFlags::SubSystemFlag flag, int iphi)
{
  assert(iphi>=-1);
  assert(m_d->subsysflag2sep.find(flag)!=m_d->subsysflag2sep.end());
  assert(m_d->subsysflag_2_iphi2sephelper.find(flag)!=m_d->subsysflag_2_iphi2sephelper.end());

  if (m_d->subsysflag_2_iphi2sephelper[flag].find(iphi)!=m_d->subsysflag_2_iphi2sephelper[flag].end()) {
    return m_d->subsysflag_2_iphi2sephelper[flag][iphi];
  }

  SoSwitch * sw = new SoSwitch;
  sw->whichChild = (iphi>=0 ? m_d->phisectionwidget->virtualSectorEnabled(iphi,NPHISECTORS):!m_d->phisectionwidget->allSectorsOff()) ? SO_SWITCH_ALL : SO_SWITCH_NONE;

  if (m_d->iphi2switches.find(iphi)==m_d->iphi2switches.end())
    m_d->iphi2switches[iphi] = std::set<SoSwitch*>();
  m_d->iphi2switches[iphi].insert(sw);

  SoSeparator * sep = new SoSeparator;
  sep->ref();
  VP1ExtraSepLayerHelper * sephelper = new VP1ExtraSepLayerHelper(sep);
  m_d->subsysflag_2_iphi2sephelper[flag][iphi] = sephelper;

  sw->addChild(sep);
  m_d->subsysflag2sep[flag]->addChild(sw);
  return sephelper;
}

//____________________________________________________________________
VP1ExtraSepLayerHelper * PhiSectorManager::getLabelSepHelperForNode(VP1GeoFlags::SubSystemFlag flag, int iphi)
{
  assert(iphi>=-1);
  assert(m_d->subsysflag2labelsep.find(flag)!=m_d->subsysflag2labelsep.end());
  assert(m_d->subsysflag_2_iphi2labelsephelper.find(flag)!=m_d->subsysflag_2_iphi2labelsephelper.end());
  
  if (m_d->subsysflag_2_iphi2labelsephelper[flag].find(iphi)!=m_d->subsysflag_2_iphi2labelsephelper[flag].end()) {
    return m_d->subsysflag_2_iphi2labelsephelper[flag][iphi];
  }
  
  SoSwitch * sw = new SoSwitch;
  sw->whichChild = (iphi>=0 ? m_d->phisectionwidget->virtualSectorEnabled(iphi,NPHISECTORS):!m_d->phisectionwidget->allSectorsOff()) ? SO_SWITCH_ALL : SO_SWITCH_NONE;
  
  if (m_d->iphi2Labelswitches.find(iphi)==m_d->iphi2Labelswitches.end())
    m_d->iphi2Labelswitches[iphi] = std::set<SoSwitch*>();
  m_d->iphi2Labelswitches[iphi].insert(sw);
  
  SoSeparator * sep = new SoSeparator;
  sep->ref();
  VP1ExtraSepLayerHelper * sephelper = new VP1ExtraSepLayerHelper(sep);
  m_d->subsysflag_2_iphi2labelsephelper[flag][iphi] = sephelper;
  
  sw->addChild(sep);
  m_d->subsysflag2labelsep[flag]->addChild(sw);
  return sephelper;
}

//____________________________________________________________________
VP1ExtraSepLayerHelper * PhiSectorManager::registerVolumeAroundZAxis( VP1GeoFlags::SubSystemFlag flag, SoSwitch* sw, const SbMatrix& transf )
{
  //Assumes that the switch, sw, has the unsliced shape node as the
  //first child (0) and a separator as the second child (1), under
  //which shapes representing the phi-sliced shape will be placed.

  //If not all phi sectors are visible, the sep node will be
  //populated with children representing the shape cut up
  //appropriately, and the switch will show the sep rather than the
  //shapeWithAllPhiSectors.

  assert(sw->getNumChildren()==2);
//   assert(sw->getChild(0)->getTypeId().isDerivedFrom(SoShape::getClassTypeId()));
  assert(sw->getChild(1)->getTypeId()==SoSeparator::getClassTypeId());

  SoNode* shape = static_cast<SoShape*>(sw->getChild(0));
  SoSeparator* sep = static_cast<SoSeparator*>(sw->getChild(1));

  //Start unsliced:
  if (sw->whichChild.getValue() != 0)
    sw->whichChild = 0;

  // Register the shape according to type:
  if (shape->getTypeId()==SoTubs::getClassTypeId()) {
    m_d->volaroundZ_tubs[sw] = std::pair<SoTubs*,SoSeparator*>(static_cast<SoTubs*>(shape),sep);
  } else if (shape->getTypeId()==SoPcons::getClassTypeId()) {
      m_d->volaroundZ_pcons[sw] = std::pair<SoPcons*,SoSeparator*>(static_cast<SoPcons*>(shape),sep);
  } else if (shape->getTypeId()==SoCons::getClassTypeId()) {
    m_d->volaroundZ_cons[sw] = std::pair<SoCons*,SoSeparator*>(static_cast<SoCons*>(shape),sep);
  } else {
    assert(shape->getTypeId()==SoLAr::getClassTypeId());
    m_d->volaroundZ_lar[sw] = std::pair<SoLAr*,SoSeparator*>(static_cast<SoLAr*>(shape),sep);
  }

  //Test: Are we roughly given by a rotation around the y-axis?
  SbVec3f unitz (0.0f, 0.0f, 1.0f);
  SbVec3f transfunitz;

  SbVec3f translation; SbRotation rotation; SbVec3f scaleFactor; SbRotation scaleOrientation;
  transf.getTransform (translation, rotation, scaleFactor, scaleOrientation);

  rotation.multVec(unitz,transfunitz);

  float x,y,z;
  transfunitz.getValue(x,y,z);
  //Look at direction in x-z plane - is it consistent with z<0 and |x|<<|z|? Then we have rotated around y:
  bool rotaroundy = (z<0&&std::fabs(x)*10<std::fabs(z));

  //Test: Whats the rough value we are rotated around z?
  SbVec3f unitx (1.0f, 0.0f, 0.0f);
  SbVec3f transfunitx;
  rotation.multVec(unitx,transfunitx);
  transfunitx.getValue(x,y,z);
  double phirot = VP1LinAlgUtils::phiFromXY(x, y );
  m_d->volaroundZ_switch2transfinfo[sw] = std::pair<double,bool>(phirot,rotaroundy);

  //Return the sephelper for this subsystems volumes around Z:
  if (m_d->subsysflag_2_volAroundZSepHelper.find(flag)!=m_d->subsysflag_2_volAroundZSepHelper.end())
    return m_d->subsysflag_2_volAroundZSepHelper[flag];

  SoSeparator * subsyssep_volaroundZ = new SoSeparator;
  assert(m_d->subsysflag2sep.find(flag)!=m_d->subsysflag2sep.end());
  m_d->subsysflag2sep[flag]->addChild(subsyssep_volaroundZ);

  subsyssep_volaroundZ->ref();
  VP1ExtraSepLayerHelper * sephelper = new VP1ExtraSepLayerHelper(subsyssep_volaroundZ);
  m_d->subsysflag_2_volAroundZSepHelper[flag] = sephelper;
  return sephelper;
}

//____________________________________________________________________
int PhiSectorManager::getVolumeType(const SbMatrix& transform, SoNode * shape) const {
  if (!shape) {
    VP1Msg::message("ERROR: getVolumeType got a NULL shape pointer");
    return 0;
  }
  
  //Step 1) Find the (projected) bounding box.
  shape->ref();
  SbViewportRegion dummyvp;
  SoGetBoundingBoxAction a(dummyvp);//Fixme: Cache this guy?
  a.apply(shape);
  SbXfBox3f xfbox = a.getXfBoundingBox();
  xfbox.transform(transform);
  SbBox3f box = xfbox.project();
  shape->unrefNoDelete();

  //Step 2) Does it enclose the Z-axis?
  float minx, miny, minz, maxx, maxy, maxz;
  box.getBounds(minx, miny, minz, maxx, maxy, maxz);
  float x,y,z;
  box.getCenter().getValue(x,y,z);
  if (std::abs(x*20.0)<std::abs(std::min(minx,maxx))&&std::abs(y*20.0)<std::abs(std::min(miny,maxy))) {
    // -> Yes: It is one of the volumes we can handle dynamically?
    if (shape->getTypeId()==SoTubs::getClassTypeId()
      ||shape->getTypeId()==SoPcons::getClassTypeId()
      ||shape->getTypeId()==SoCons::getClassTypeId()
      ||shape->getTypeId()==SoLAr::getClassTypeId()) {
      // -> Yes:
      return -2;
    } else {
      // -> No:
      //Fixme: Move this message somewhere appropriate???
      //shape cannot be NULL here, so 'false' case is redundant (coverity 16272)
      //std::string Typenametest = shape ? shape->getTypeId().getName().getString() : "NULL";
      std::string Typenametest = shape->getTypeId().getName().getString();
      m_d->system->message("WARNING: Unknown volume type (boolean?) for volume around Z-axis (type "
			 +QString(Typenametest.c_str())+"). Phi-sector cuts won't work for this!");
      return -1;
    }
  }

  //Step 3: Find phi of center and return appropriate iphi:
  return PhiSectionWidget::phiToVirtualSectorIndex(VP1LinAlgUtils::phiFromXY(x,y),NPHISECTORS);

}

//____________________________________________________________________
PhiSectorManager::PhiSectorManager(PhiSectionWidget * psw, IVP1System * sys, QObject*parent)
  : QObject(parent), m_d(new Imp)
{
  m_d->phisectionwidget = psw;
  m_d->system=sys;
  m_d->nactivelargechanges = 0;
  m_d->currentlyEnabledPhiSectors = QVector<bool>(NPHISECTORS,false);
  QList<int> l; l << 4 << 6 << 9 << 12 << 18 << NPHISECTORS;//NB: All must be divisors in NPHISECTORS
  psw->setNumberOfSectors(12);
  psw->setAllowedNumberOfSectors(l);

  connect(psw,SIGNAL(enabledPhiRangesChanged(const QList<VP1Interval>&)),this,SLOT(enabledPhiSectorsChanged()));
  enabledPhiSectorsChanged();
}

//____________________________________________________________________
PhiSectorManager::~PhiSectorManager()
{

  std::map<VP1GeoFlags::SubSystemFlag,std::map<int,VP1ExtraSepLayerHelper*> >::iterator it, itE = m_d->subsysflag_2_iphi2sephelper.end();
  for (it = m_d->subsysflag_2_iphi2sephelper.begin();it!=itE;++it) {
    std::map<int,VP1ExtraSepLayerHelper*>::iterator it2(it->second.begin()),it2E(it->second.end());
    for (;it2!=it2E;++it2) {
      SoSeparator * sep = it2->second->topSeparator();
      delete (it2->second);
      sep->unref();
    }
  }
  
  std::map<VP1GeoFlags::SubSystemFlag,std::map<int,VP1ExtraSepLayerHelper*> >::iterator it4, itE4 = m_d->subsysflag_2_iphi2labelsephelper.end();
  for (it4 = m_d->subsysflag_2_iphi2labelsephelper.begin();it4!=itE4;++it4) {
    std::map<int,VP1ExtraSepLayerHelper*>::iterator it5(it4->second.begin()),it5E(it4->second.end());
    for (;it5!=it5E;++it5) {
      SoSeparator * sep = it5->second->topSeparator();
      delete (it5->second);
      sep->unref();
    }
  }
  std::map<VP1GeoFlags::SubSystemFlag,VP1ExtraSepLayerHelper*>::iterator it3,it3E = m_d->subsysflag_2_volAroundZSepHelper.end();
  for (it3 = m_d->subsysflag_2_volAroundZSepHelper.begin();it3!=it3E;++it3) {
    SoSeparator * sep = it3->second->topSeparator();
    delete (it3->second);
    sep->unref();
  }
  delete m_d; m_d=0;
}

//_____________________________________________________________________________________
void PhiSectorManager::enabledPhiSectorsChanged()
{
  QVector<bool> v =  m_d->phisectionwidget->virtualSectorsEnabled(NPHISECTORS);
  if (m_d->currentlyEnabledPhiSectors == v)
    return;
  QList<int> changedPhiSectors;
  for (int iphi = 0; iphi < NPHISECTORS; ++iphi)
    if (m_d->currentlyEnabledPhiSectors[iphi]!=v[iphi])
      changedPhiSectors << iphi;
  m_d->currentlyEnabledPhiSectors = v;
  foreach (int iphi,changedPhiSectors)
    updateEnabledPhiSections(iphi);
}

//_____________________________________________________________________________________
void PhiSectorManager::updateEnabledPhiSections(int iphi) {
  bool turnedon = m_d->currentlyEnabledPhiSectors.at(iphi);
  if (m_d->iphi2switches.find(iphi)!=m_d->iphi2switches.end()) {
    std::set<SoSwitch*>::iterator it, itE = m_d->iphi2switches[iphi].end();
    for (it = m_d->iphi2switches[iphi].begin();it!=itE;++it) {
      if ((*it)->whichChild.getValue() != (turnedon?SO_SWITCH_ALL:SO_SWITCH_NONE))
        (*it)->whichChild = (turnedon?SO_SWITCH_ALL:SO_SWITCH_NONE);
    }
  }
  
  if (m_d->iphi2Labelswitches.find(iphi)!=m_d->iphi2Labelswitches.end()) {
    std::set<SoSwitch*>::iterator it, itE = m_d->iphi2Labelswitches[iphi].end();
    for (it = m_d->iphi2Labelswitches[iphi].begin();it!=itE;++it) {
      if ((*it)->whichChild.getValue() != (turnedon?SO_SWITCH_ALL:SO_SWITCH_NONE))
        (*it)->whichChild = (turnedon?SO_SWITCH_ALL:SO_SWITCH_NONE);
    }
  }
  // FIXME - need to find a way to turn on labels for recently made visible phi sections, OR build them all by default. 
  
  //Update the switches for boolean, etc., volumes around the Z axis.
  bool alloff = m_d->phisectionwidget->allSectorsOff();
  std::map<int,std::set<SoSwitch*> >::iterator it_zax_switches = m_d->iphi2switches.find(-1);
  if (it_zax_switches!=m_d->iphi2switches.end()) {
    std::set<SoSwitch*>::iterator it, itE = it_zax_switches->second.end();
    for (it = it_zax_switches->second.begin();it!=itE;++it) {
      if ((*it)->whichChild.getValue() != (alloff?SO_SWITCH_NONE:SO_SWITCH_ALL))
        (*it)->whichChild = (alloff?SO_SWITCH_NONE:SO_SWITCH_ALL);
    }
  }
  updateRepresentationsOfVolsAroundZAxis();
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::getPhiExtentOfShape( const SoTubs* tubs, double&  phistart, double& phispan)
{
  phistart = tubs->pSPhi.getValue();
  phispan  = tubs->pDPhi.getValue();
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::copyShapeFieldsAndOverridePhiRange( const SoTubs* source, SoTubs* dest, double phistart, double phispan)
{
  dest->pRMin.setValue(source->pRMin.getValue());
  dest->pRMax.setValue(source->pRMax.getValue());
  dest->pDz.setValue(source->pDz.getValue());
  dest->pSPhi.setValue(phistart);
  dest->pDPhi.setValue(phispan);
  dest->drawEdgeLines.setValue(source->drawEdgeLines.getValue());
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::getPhiExtentOfShape( const SoPcons* pcons, double&  phistart, double& phispan)
{
  phistart = pcons->fSPhi.getValue();
  phispan  = pcons->fDPhi.getValue();
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::copyShapeFieldsAndOverridePhiRange( const SoPcons* source, SoPcons* dest, double phistart, double phispan)
{
  dest->fRmin = source->fRmin;
  dest->fRmax = source->fRmax;
  dest->fDz = source->fDz;
  dest->fSPhi.setValue(phistart);
  dest->fDPhi.setValue(phispan);
  dest->drawEdgeLines.setValue(source->drawEdgeLines.getValue());
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::getPhiExtentOfShape( const SoCons* cons, double&  phistart, double& phispan)
{
  phistart = cons->fSPhi.getValue();
  phispan  = cons->fDPhi.getValue();
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::copyShapeFieldsAndOverridePhiRange( const SoCons* source, SoCons* dest, double phistart, double phispan)
{
  dest->fRmin1.setValue(source->fRmin1.getValue());
  dest->fRmin2.setValue(source->fRmin2.getValue());
  dest->fRmax1.setValue(source->fRmax1.getValue());
  dest->fRmax2.setValue(source->fRmax2.getValue());
  dest->fDz.setValue(source->fDz.getValue());
  dest->fSPhi.setValue(phistart);
  dest->fDPhi.setValue(phispan);
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::getPhiExtentOfShape( const SoLAr* lar, double&  phistart, double& phispan)
{
  phistart = lar->fSPhi.getValue();
  phispan  = lar->fDPhi.getValue();
}

//_____________________________________________________________________________________
void PhiSectorManager::Imp::copyShapeFieldsAndOverridePhiRange( const SoLAr* source, SoLAr* dest, double phistart, double phispan)
{
  dest->fRmin = source->fRmin;
  dest->fRmax = source->fRmax;
  dest->fDz = source->fDz;
  dest->fSPhi.setValue(phistart);
  dest->fDPhi.setValue(phispan);
}

//_____________________________________________________________________________________
template <class T>
void PhiSectorManager::Imp::updateRepresentationsOfVolsAroundZAxis_Specific(std::map<SoSwitch*,std::pair<T*,SoSeparator*> >& volaroundZ)
{
  
  typename std::map<SoSwitch*,std::pair<T*,SoSeparator*> >::iterator it, itE = volaroundZ.end();
  for (it = volaroundZ.begin();it!=itE;++it) {
    if (phisectionwidget->allSectorsOff()) {
      if (it->first->whichChild.getValue() != SO_SWITCH_NONE)
        it->first->whichChild = SO_SWITCH_NONE;
    } else if (phisectionwidget->allSectorsOn()) {
      if (it->first->whichChild.getValue() != 0)
        it->first->whichChild = 0;
    } else {
      //Update and use sep node:
      SbBool save = it->first->enableNotify(false);
      SoSeparator * sep = it->second.second;
      sep->removeAllChildren();
      //Figure out phirot/yrot info:
      std::map<SoSwitch*,std::pair<double,bool> >::iterator it_transfinfo = volaroundZ_switch2transfinfo.find(it->first);
      if (it_transfinfo==volaroundZ_switch2transfinfo.end()) {
        system->message("updateRepresentationsOfVolsAroundZAxis_Specific ERROR: Inconsistent maps");
        continue;
      }
      double phirot = it_transfinfo->second.first;
      bool rotaroundy = it_transfinfo->second.second;
      
      double sphi, dphi;
      getPhiExtentOfShape(it->second.first,sphi,dphi);
      
      QList<VP1Interval> ranges = phisectionwidget->enabledPhiRanges( sphi, sphi+dphi );
      
      int n = ranges.count();
      for (int i = 0; i< n; ++i) {
        T * shape = new T;
        double deltaphi = ranges.at(i).length();
        double startphi = rotaroundy ? - ranges.at(i).upper() - 2*M_PI + phirot : ranges.at(i).lower()-phirot;
        while (startphi<0) startphi += 2*M_PI;
        while (startphi+deltaphi>2*M_PI) startphi -= 2*M_PI;
        copyShapeFieldsAndOverridePhiRange( it->second.first, shape, startphi, deltaphi );
        sep->addChild(shape);
      }
      
      it->first->whichChild = 1;
      it->first->enableNotify(save);
      if (save)
        it->first->touch();
      
    }
  }
  
}

//_____________________________________________________________________________________
void PhiSectorManager::updateRepresentationsOfVolsAroundZAxis()
{
  m_d->updateRepresentationsOfVolsAroundZAxis_Specific<SoTubs>(m_d->volaroundZ_tubs);
  m_d->updateRepresentationsOfVolsAroundZAxis_Specific<SoPcons>(m_d->volaroundZ_pcons);
  m_d->updateRepresentationsOfVolsAroundZAxis_Specific<SoCons>(m_d->volaroundZ_cons);
  m_d->updateRepresentationsOfVolsAroundZAxis_Specific<SoLAr>(m_d->volaroundZ_lar);
}

//_____________________________________________________________________________________
void PhiSectorManager::largeChangesBegin()
{
  if (++(m_d->nactivelargechanges)==1) {
    VP1Msg::messageVerbose("PhiSectorManager disabling notifications begin");
    std::map<VP1GeoFlags::SubSystemFlag,std::map<int,VP1ExtraSepLayerHelper*> >::iterator it, itE = m_d->subsysflag_2_iphi2sephelper.end();
    for (it = m_d->subsysflag_2_iphi2sephelper.begin();it!=itE;++it) {
      std::map<int,VP1ExtraSepLayerHelper*>::iterator it2(it->second.begin()),it2E(it->second.end());
      for (;it2!=it2E;++it2)
        it2->second->largeChangesBegin();
    }
    std::map<VP1GeoFlags::SubSystemFlag,VP1ExtraSepLayerHelper*>::iterator it3,it3E = m_d->subsysflag_2_volAroundZSepHelper.end();
    for (it3 = m_d->subsysflag_2_volAroundZSepHelper.begin();it3!=it3E;++it3)
      it3->second->largeChangesBegin();
    VP1Msg::messageVerbose("PhiSectorManager disabling notifications end");
  }
}

//_____________________________________________________________________________________
void PhiSectorManager::largeChangesEnd()
{
  if (--(m_d->nactivelargechanges)==0) {
    VP1Msg::messageVerbose("PhiSectorManager enabling notifications begin");
    std::map<VP1GeoFlags::SubSystemFlag,std::map<int,VP1ExtraSepLayerHelper*> >::iterator it, itE = m_d->subsysflag_2_iphi2sephelper.end();
    for (it = m_d->subsysflag_2_iphi2sephelper.begin();it!=itE;++it) {
      std::map<int,VP1ExtraSepLayerHelper*>::iterator it2(it->second.begin()),it2E(it->second.end());
      for (;it2!=it2E;++it2)
        it2->second->largeChangesEnd();
    }
    std::map<VP1GeoFlags::SubSystemFlag,VP1ExtraSepLayerHelper*>::iterator it3,it3E = m_d->subsysflag_2_volAroundZSepHelper.end();
    for (it3 = m_d->subsysflag_2_volAroundZSepHelper.begin();it3!=it3E;++it3)
      it3->second->largeChangesEnd();
    VP1Msg::messageVerbose("PhiSectorManager enabling notifications end");
  }
}
