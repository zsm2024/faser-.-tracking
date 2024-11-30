/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class TrackHandle_TrkTrack              //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: February 2008                            //
//                                                            //
////////////////////////////////////////////////////////////////

#include "VTI12TrackSystems/TrackHandle_TrkTrack.h"
#include "VTI12TrackSystems/TrackSystemController.h"
#include "VTI12TrackSystems/TrackCollHandle_TrkTrack.h"
#include "VTI12TrackSystems/VP1TrackSanity.h"
#include "VTI12TrackSystems/TrkObjToString.h"
#include "VTI12Utils/VP1JobConfigInfo.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "VP1Base/VP1Msg.h"//fixme
#include "VTI12TrackSystems/VP1TrackSummary.h"
#include "VTI12TrackSystems/AscObj_TSOS.h"
#include "VTI12TrackSystems/TrkObjToString.h"
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoNode.h>


//____________________________________________________________________
TrackHandle_TrkTrack::TrackHandle_TrkTrack(TrackCollHandleBase* ch, const Trk::Track* trk)
  : TrackHandleBase(ch),
    m_nhits_sct(0), m_hitInfoStatus(-1), m_trk(trk)
{
}

//____________________________________________________________________
TrackHandle_TrkTrack::~TrackHandle_TrkTrack()
{
}

//____________________________________________________________________
QStringList TrackHandle_TrkTrack::clicked() const
{
  ensureInitSubSysHitInfo();
    // Object browser stuff
  QTreeWidget* trkObjBrowser = common()->controller()->trackObjBrowser();
  if (trkObjBrowser){    
    trkObjBrowser->setCurrentItem(browserTreeItem());
    trkObjBrowser->scrollToItem(browserTreeItem());
  }
  QStringList l;
  l << "Trk::Track";
  if (!m_trk) {
    l << "ERROR: This TrackHandle_TrkTrack handle has a NULL track pointer!!";
    return l;
  }
  l << "Author info: " << "  "+QString(m_trk->info().dumpInfo().c_str());

  l << TrackHandleBase::baseInfo();
  if (m_trk->fitQuality()) 
    l << "Fit quality [Chi^2, nDoF] = ["+QString::number(m_trk->fitQuality()->chiSquared())+", "+QString::number(m_trk->fitQuality()->doubleNumberDoF())+"]. ";
  if (common()->controller()->printVerboseInfoOnSingleSelection()) {
    l << " ===== dump =====";
    std::ostringstream s;
    s << *m_trk;
    l << QString(s.str().c_str()).split('\n');
  }
  return l;
}

//____________________________________________________________________
Amg::Vector3D TrackHandle_TrkTrack::momentum() const
{
  for (const Trk::TrackParameters* param : *m_trk->trackParameters()) {
    if (!common()->trackSanityHelper()->isSafe(param))
      continue;
    return param->momentum();
  }
  return TrackHandleBase::momentum();
}

//____________________________________________________________________
void TrackHandle_TrkTrack::visibleStateChanged()
{
  static_cast<TrackCollHandle_TrkTrack*>(collHandle())->visibleStateUpdated(this);
  TrackHandleBase::visibleStateChanged();
}

//____________________________________________________________________
void TrackHandle_TrkTrack::currentMaterialChanged()
{
  if (visible())
    static_cast<TrackCollHandle_TrkTrack*>(collHandle())->visibleStateUpdated(this);
}

//____________________________________________________________________
void TrackHandle_TrkTrack::ensureInitSubSysHitInfo() const
{
  if (m_hitInfoStatus==-1) 
    m_hitInfoStatus = VP1TrackSummary::countHits( m_trk, m_nhits_sct) ? 1 : 0;
}

void TrackHandle_TrkTrack::fillObjectBrowser( QList<QTreeWidgetItem *>& listOfItems) 
{
  TrackHandleBase::fillObjectBrowser(listOfItems); // Obligatory!
   
  // Fill sub-data.
  QList<AssociatedObjectHandleBase*> list = getAllAscObjHandles();
  DataVector< const Trk::TrackStateOnSurface >::const_iterator it  = trkTrackPointer()->trackStateOnSurfaces ()->begin(), 
  itE = trkTrackPointer()->trackStateOnSurfaces ()->end();
  unsigned int i=0;
  for (;it!=itE;++it){
//    VP1Msg::messageVerbose("TSOS #"+QString::number(i++));

    bool visible=false;
    QTreeWidgetItem* TSOSitem = new QTreeWidgetItem(browserTreeItem());
    
    AscObj_TSOS* asc=0;
    if (i<static_cast<unsigned int>(list.size())) asc = dynamic_cast<AscObj_TSOS*>(list.at(i));

    if (asc) {
      asc->setBrowserTreeItem(TSOSitem);
      visible=asc->visible();
    }
    
    if (!visible) {
      TSOSitem->setFlags(0);// not selectable, not enabled
      QFont itemFont = TSOSitem->font(0);
      itemFont.setStrikeOut(true);
      TSOSitem->setFont(0, itemFont);
      TSOSitem->setFont(1, itemFont);
    } 
    TSOSitem->setText(0, QString("Track State "+QString::number( (int)(it - trkTrackPointer()->trackStateOnSurfaces ()->begin() ) ) ) );
    
    QString tsosText;      
    bool first=true;
    const Trk::MeasurementBase* meas = (*it)->measurementOnTrack();
    if ( /**(*it)->type(Trk::TrackStateOnSurface::Measurement) &&*/ meas ){
      QString measName(TrkObjToString::name(*meas));
      tsosText+=measName;
      
      QTreeWidgetItem* measItem = new QTreeWidgetItem(TSOSitem);
      measItem->setExpanded(true); // want it opened so subparams seen easily
      measItem->setFlags(Qt::ItemIsEnabled);
      measItem->setText(0, measName);
      // measItem->setText(1, TrkObjToString::shortInfo(*meas) );
      
      QStringList list = TrkObjToString::fullInfo( *((*it)->measurementOnTrack ()) );
      for (int i = 0; i < (list.size()-1); ){
        QTreeWidgetItem* subparamItem = new QTreeWidgetItem(measItem);
        subparamItem->setText(0, list.at(i++) );
        subparamItem->setText(1, list.at(i++) );   
        subparamItem->setFlags(Qt::ItemIsEnabled); 
      }
      
      first=false;
    }
    if ( (*it)->trackParameters () ) {
      if (!first) tsosText.append(" + ");
      tsosText.append("Parameters");
      
      QTreeWidgetItem* paramItem = new QTreeWidgetItem(TSOSitem);
      paramItem->setExpanded(true); // want it opened so subparams seen easily
      paramItem->setFlags(Qt::ItemIsEnabled); 
      paramItem->setText(0, TrkObjToString::name(      *((*it)->trackParameters ()) ) );
      // paramItem->setText(1, TrkObjToString::shortInfo( *((*it)->trackParameters ()) ) );
      
      QStringList list = TrkObjToString::fullInfo( *((*it)->trackParameters ()) );
      for (int i = 0; i < (list.size()-1); ){
        QTreeWidgetItem* subparamItem = new QTreeWidgetItem(paramItem);
        subparamItem->setText(0, list.at(i++) );
        subparamItem->setText(1, list.at(i++) ); 
        subparamItem->setFlags(Qt::ItemIsEnabled);   
      }
      
      first=false;
    }
    if ( (*it)->materialEffectsOnTrack () ){
      if (!first) tsosText.append(" + ");
      tsosText.append("MaterialEffectsOnTrack"); 
      QTreeWidgetItem* meItem = new QTreeWidgetItem(TSOSitem);
      meItem->setExpanded(true); // want it opened so subparams seen easily
      meItem->setFlags(Qt::ItemIsEnabled);
      
      meItem->setText(0, TrkObjToString::name(      *((*it)->materialEffectsOnTrack ()) ) );
      meItem->setText(1, TrkObjToString::shortInfo( *((*it)->materialEffectsOnTrack ()) ) );
      
      first=false;
    }
    if ( (*it)->fitQualityOnSurface () ){ 
      if (!first) tsosText.append(" + ");
      tsosText.append("FitQuality");
      QTreeWidgetItem* fqItem = new QTreeWidgetItem(TSOSitem);
      fqItem->setExpanded(true); // want it opened so subparams seen easily
      fqItem->setFlags(Qt::ItemIsEnabled);
      fqItem->setText(0,       QString("FitQuality") );
      fqItem->setText(1, TrkObjToString::shortInfo( ((*it)->fitQualityOnSurface ()) ) );
      
      first=false;
    }
    // FIXME - add information about chamber for Muon systems?
    
    if ( (*it)->type(Trk::TrackStateOnSurface::Outlier) ){ 
      if (!first) tsosText.append(" + ");
      tsosText.append("Outlier"); 
    }
    
    if ( (*it)->type(Trk::TrackStateOnSurface::InertMaterial) ){ 
      if (!first) tsosText.append(" + ");
      tsosText.append("InertMaterial"); 
    }
    
    if ( (*it)->type(Trk::TrackStateOnSurface::BremPoint) ){ 
      if (!first) tsosText.append(" + ");
      tsosText.append("BremPoint"); 
    }
    
    if ( (*it)->type(Trk::TrackStateOnSurface::Perigee) ){ 
      if (!first) tsosText.append(" + ");
      tsosText.append("Perigee"); 
    }
    
    if ( (*it)->type(Trk::TrackStateOnSurface::Hole) ){ 
      if (!first) tsosText.append(" + ");
      tsosText.append("Hole"); 
    }
    TSOSitem->setText(1, tsosText );
  }
  // listOfItems << browserTreeItem();
}

SoNode* TrackHandle_TrkTrack::zoomToTSOS(unsigned int index)
{
  if (trkTrackPointer() && (index<trkTrackPointer()->trackStateOnSurfaces()->size() )){
    const Trk::TrackStateOnSurface* tsos = (*trkTrackPointer()->trackStateOnSurfaces())[index];
    // now find matching AscObj_TSOS
    QList<AssociatedObjectHandleBase*> list = getAllAscObjHandles();
    for (int i = 0; i < list.size(); ++i) {
      // VP1Msg::messageVerbose("TrackSystemController::objectBrowserClicked: checking ASC "+QString::number(i));
      
      AscObj_TSOS* asc = dynamic_cast<AscObj_TSOS*>(list.at(i));
      if (asc) {
        if (asc && asc->trackStateOnSurface()==tsos) {
           VP1Msg::messageVerbose("TrackSystemController::objectBrowserClicked: this ASC matches "+QString::number(i));
          //asc->zoomView();
          return asc->shapeDetailed();
        }
      }
    }
  }
  return 0;
}

QString TrackHandle_TrkTrack::shortInfo() const
{
  QString l("|P|="+VP1Msg::str(momentum().mag()/CLHEP::GeV)+" [GeV], ");
  l+= "SCT["+QString::number(getNSCTHits())+"]";
  return l;
}


void TrackHandle_TrkTrack::updateObjectBrowser(){
  
  if ( !browserTreeItem()) {
    VP1Msg::messageVerbose("updateObjectBrowser: No m_objBrowseTree!");
    return;
  }
  
  // QString tmp = (visible()==true)?"True":"False";
//  VP1Msg::messageVerbose("updateObjectBrowser  "+m_objBrowseTree->text(0)+" with visible()="+tmp);
  
  if (!visible()) {
    browserTreeItem()->setFlags(0); // not selectable, not enabled
  } else {
    browserTreeItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
  }
  QFont font= browserTreeItem()->font(0);
  font.setStrikeOut(!visible());
  browserTreeItem()->setFont(0, font );
  browserTreeItem()->setFont(1, font );

  
  // FIXME! Only do if necessary i.e. if something affecting this TSOS has changed.
  
  QList<AssociatedObjectHandleBase*> list = getAllAscObjHandles();
  
  if (list.size()==0) {
    VP1Msg::message("No ASC objects associated with this track - no track components visible yet?");
    return;
  }
    
  DataVector< const Trk::TrackStateOnSurface >::const_iterator it  = trkTrackPointer()->trackStateOnSurfaces ()->begin(), 
  itE = trkTrackPointer()->trackStateOnSurfaces ()->end();
  unsigned int numOfTSOS=0;
  for (;it!=itE;++it,++numOfTSOS){
    AscObj_TSOS* asc = dynamic_cast<AscObj_TSOS*>(list.at(numOfTSOS));
    if (!(asc && asc->trackStateOnSurface()==*it && list.size()>0) ) {
      // Fallback.
      VP1Msg::message("TSOS#"+QString::number(numOfTSOS)+" does not seem to match AscObj_TSOS in list, which has size="+QString::number(list.size())+" Will try fallback.");
      for (int i = 0; i < list.size(); ++i) {          
        AscObj_TSOS* asc = dynamic_cast<AscObj_TSOS*>(list.at(i));
        if (asc && asc->trackStateOnSurface()==*it) {
          VP1Msg::message("-> this ASC matches "+QString::number(i));
        } 
      }      
    }
    if (!asc) {
      VP1Msg::message("Could not find matching Asc in list of size "+QString::number(list.size()));
      continue;
    }
    
    if (!asc->browserTreeItem() && browserTreeItem()){
      // not set yet - so need to do this now. Can we just use the index?
      asc->setBrowserTreeItem(browserTreeItem()->child(numOfTSOS));
    }
    
    if (!asc->browserTreeItem()){
      VP1Msg::message("Could not find matching browserTreeItem");
      continue;
    }
    
    if (!asc->visible()) {
      asc->browserTreeItem()->setFlags(0); // not selectable, not enabled
    } else {
      asc->browserTreeItem()->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled); //  selectable,  enabled
    }    
    QFont itemFont = asc->browserTreeItem()->font(0);
    itemFont.setStrikeOut(!asc->visible());
    asc->browserTreeItem()->setFont(0, itemFont);
    asc->browserTreeItem()->setFont(1, itemFont);
  }
}

const Trk::FitQuality* TrackHandle_TrkTrack::getFitQuality() const {
  return m_trk ? m_trk->fitQuality ():0;
}

