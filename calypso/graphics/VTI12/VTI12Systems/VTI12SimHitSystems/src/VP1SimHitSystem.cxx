/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VTI12SimHitSystems/VP1SimHitSystem.h"
#include "ui_simhitcontrollerform.h"

#include "StoreGate/StoreGateSvc.h"

#include <QMap>
#include <QSet>

#include <Inventor/C/errors/debugerror.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoVertexProperty.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/SbColor.h>

#include "FaserCaloSimEvent/CaloHitCollection.h"
#include "ScintSimEvent/ScintHitCollection.h"
#include "FaserGeoAdaptors/GeoScintHit.h"
#include "NeutrinoSimEvent/NeutrinoHitCollection.h"
#include "FaserGeoAdaptors/GeoNeutrinoHit.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "FaserGeoAdaptors/GeoFaserSiHit.h"
#include "FaserGeoAdaptors/GeoFaserCaloHit.h"


#include "VTI12Utils/VP1SGContentsHelper.h"

#include "GeoPrimitives/GeoPrimitives.h"

class VP1SimHitSystem::Clockwork
{
public:
  QMap<QString,SoSwitch*>    switchMap;
  QMap<QString,SbColor>      colorMap;
  QMap<QCheckBox*,QString>   checkBoxNamesMap;
  QMap<QString,QCheckBox*>   checkBoxMap;
  QSet<QString>              hitsThisEvent;
  StoreGateSvc * sg;
};

VP1SimHitSystem::VP1SimHitSystem()
  :IVP13DSystemSimple("Sim Hits","Display simulation hits","Vakho Tsulaia <Vakhtang.Tsulaia@cern.ch>"),
   m_clockwork(new Clockwork())
{
}

VP1SimHitSystem::~VP1SimHitSystem()
{
  delete m_clockwork;
  m_clockwork = 0;
}

QWidget* VP1SimHitSystem::buildController()
{
  QWidget* controller = new QWidget(0);
  Ui::SimHitControllerForm ui;
  ui.setupUi(controller);

  // Populate Check Box Names Map
  m_clockwork->checkBoxNamesMap.insert(ui.chbxEmulsionHits,"Emulsion");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxVetoNuHits,"VetoNu");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxVetoHits,"Veto");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxTriggerHits,"Trigger");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxPreshowerHits,"Preshower");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxTrackerHits,"Tracker");
  m_clockwork->checkBoxNamesMap.insert(ui.chbxCaloHits,"Calorimeter");

  // Populate Check Box Map and connect slots
  foreach(QCheckBox* cb,m_clockwork->checkBoxNamesMap.keys())
  {
    connect(cb,SIGNAL(toggled(bool)),this,SLOT(checkboxChanged()));
    m_clockwork->checkBoxMap.insert(m_clockwork->checkBoxNamesMap[cb],cb);
  }

  return controller;
}

void VP1SimHitSystem::systemcreate(StoreGateSvc* /*detstore*/)
{
  // Populate Color Map
  m_clockwork->colorMap.insert("Emulsion",SbColor(1,0,1));
  m_clockwork->colorMap.insert("VetoNu",SbColor(0,1,1));
  m_clockwork->colorMap.insert("Veto",SbColor(0,0,1));
  m_clockwork->colorMap.insert("Trigger",SbColor(1,1,1));
  m_clockwork->colorMap.insert("Preshower",SbColor(1,0,0));
  m_clockwork->colorMap.insert("Tracker",SbColor(.98,.8,.21));
  m_clockwork->colorMap.insert("Calorimeter",SbColor(0,.44,.28));
}

void VP1SimHitSystem::buildEventSceneGraph(StoreGateSvc* sg, SoSeparator *root)
{
  // --- Draw style: POINTS
  SoDrawStyle* ds = new SoDrawStyle();
  ds->style.setValue(SoDrawStyle::POINTS);
  ds->pointSize=4;
  root->addChild(ds);

  // Keep SG pointer
  m_clockwork->sg = sg;

  // clean up
  m_clockwork->switchMap.clear();
  m_clockwork->hitsThisEvent.clear();

  if(!sg)
  {
    message("0 pointer to the Store Gate!");
    return;
  }

  foreach(QString detector,m_clockwork->checkBoxMap.keys())
  {
    // Add switch, off by default
    SoSwitch* sw = new SoSwitch();
    sw->whichChild = SO_SWITCH_NONE;
    root->addChild(sw);
    m_clockwork->switchMap.insert(detector,sw);

    // Build subtree if the check box is ON
    if(m_clockwork->checkBoxMap.contains(detector) &&
       m_clockwork->checkBoxMap[detector]->isChecked())
    {
      buildHitTree(detector);
      sw->whichChild = SO_SWITCH_ALL;
    }
    updateGUI();
  }
}

void VP1SimHitSystem::checkboxChanged()
{
  // Get ChB pointer
  QCheckBox* cb = dynamic_cast<QCheckBox*>(sender());
  if(cb && m_clockwork->checkBoxNamesMap.contains(cb))
  {
    // Get detector name
    QString detector = m_clockwork->checkBoxNamesMap[cb];
    if(m_clockwork->switchMap.contains(detector))
    {
      // Get swtich
      SoSwitch* sw = m_clockwork->switchMap[detector];
      if(cb->isChecked()){
  // Build subtree if necessary
        if(!m_clockwork->hitsThisEvent.contains(detector))
          buildHitTree(detector);
        sw->whichChild = SO_SWITCH_ALL;
      }
      else
        sw->whichChild = SO_SWITCH_NONE;
    } else {
      message("WARNING: Unknown detector:"+detector);
    }
  }
}


void VP1SimHitSystem::buildHitTree(const QString& detector)
{
  messageVerbose("buildHitTree for "+detector);
  if(m_clockwork->hitsThisEvent.contains(detector)) {
    messageVerbose(" in hitsThisEvent"); return;
  }
  if(!m_clockwork->colorMap.contains(detector)) {
    messageVerbose("not in colorMap"); return;
  }
  if(!m_clockwork->switchMap.contains(detector)) {
    messageVerbose("not in switchMap"); return;
  }

  // -- Initializations
  StoreGateSvc* sg = m_clockwork->sg;
  SoSwitch* sw = m_clockwork->switchMap[detector];
  unsigned int hitCount = 0;

  SoVertexProperty* hitVtxProperty = new SoVertexProperty();
  SoPointSet* hitPointSet    = new SoPointSet();

  hitVtxProperty->enableNotify(FALSE);
  hitPointSet->enableNotify(FALSE);

  // -- COLOR
  SoMaterial* material = new SoMaterial();
  material->diffuseColor.setValue(m_clockwork->colorMap[detector]);

  sw->addChild(material);

  // Take hits from SG
  if(detector=="Emulsion")
  {
    //
    // Emulsion:
    //
    const NeutrinoHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection, "EmulsionHits")==StatusCode::SUCCESS)
    {
      for(NeutrinoHitConstIterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit)
      {
        GeoNeutrinoHit ghit(*i_hit);
        if(!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve Emulsion Hits");
  }
  else if(detector=="Veto")
  {
    //
    // Veto:
    //
    const ScintHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection, "VetoHits")==StatusCode::SUCCESS)
    {
      for(ScintHitConstIterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit)
      {
        GeoScintHit ghit(*i_hit);
        if(!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve Veto Hits");
  }
  else if(detector=="VetoNu")
  {
    //
    // VetoNu:
    //
    const ScintHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection, "VetoNuHits")==StatusCode::SUCCESS)
    {
      for(ScintHitConstIterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit)
      {
        GeoScintHit ghit(*i_hit);
        if(!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve VetoNu Hits");
  }
  else if(detector=="Trigger")
  {
    //
    // Trigger:
    //
    const ScintHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection,"TriggerHits")==StatusCode::SUCCESS)
    {
      for(ScintHitConstIterator i_hit=p_collection->begin();i_hit!=p_collection->end();++i_hit)
      {
        GeoScintHit ghit(*i_hit);
        if (!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve Trigger Hits");
  }
  else if(detector=="Preshower")
  {
    //
    // Preshower:
    //
    const ScintHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection,"PreshowerHits")==StatusCode::SUCCESS)
    {
      for(ScintHitConstIterator i_hit=p_collection->begin();i_hit!=p_collection->end();++i_hit)
      {
        GeoScintHit ghit(*i_hit);
        if(!ghit) continue;
        Amg::Vector3D u = Amg::Hep3VectorToEigen(ghit.getGlobalPosition());
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(), u.z() );
      }
    }
    else
      message("Unable to retrieve Preshower Hits");
  }
  else if(detector=="Ecal" || detector =="Calorimeter")
  {
    //
    // Ecal:
    //
    const CaloHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection,"EcalHits")==StatusCode::SUCCESS)
    {
      for(CaloHitConstIterator i_hit=p_collection->begin();i_hit!=p_collection->end();++i_hit)
      {
        GeoFaserCaloHit ghit(*i_hit);
        if(!ghit) continue;
        Amg::Vector3D u = Amg::Hep3VectorToEigen(ghit.getGlobalPosition());
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(), u.z() );
      }
    }
    else
      message("Unable to retrieve Ecal Hits");
  }
  else if (detector=="Tracker")
  {
    const FaserSiHitCollection* p_collection = nullptr;
    if(sg->retrieve(p_collection, "SCT_Hits")==StatusCode::SUCCESS)
    {
      std::cout<<"Retrieved Si hit container with " << p_collection->size() << " hits" << std::endl;
      for(FaserSiHitConstIterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit)
      {
        GeoFaserSiHit ghit(*i_hit);
        if(!ghit) continue;
        HepGeom::Point3D<double> u = ghit.getGlobalPosition();
        hitVtxProperty->vertex.set1Value(hitCount++,u.x(),u.y(),u.z());
      }
    }
    else
      message("Unable to retrieve SCT Hits");
  }
  // Add to the switch
  hitPointSet->numPoints=hitCount;
  hitPointSet->vertexProperty.setValue(hitVtxProperty);
  sw->addChild(hitPointSet);
  hitPointSet->enableNotify(TRUE);
  hitVtxProperty->enableNotify(TRUE);
}

