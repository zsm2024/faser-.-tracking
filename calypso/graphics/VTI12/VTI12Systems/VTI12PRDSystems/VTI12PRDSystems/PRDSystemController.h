/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class PRDSystemController                 //
//                                                            //
//  Description: Controller widget for the PRD system.        //
//               Keeps all nasty gui stuff internally, and    //
//               only presents the actual interesting data    //
//               with specialised access methods and signals. //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef PRDSYSTEMCONTROLLER_H
#define PRDSYSTEMCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include "VTI12PRDSystems/PRDCollHandleBase.h"
#include "VTI12PRDSystems/PRDCommonFlags.h"
#include "VTI12PRDSystems/PRDDetTypes.h"
// #include "VTI12Utils/InDetProjFlags.h"
#include <QByteArray>
#include <set>
class VP1CollectionWidget;
class SoMaterial;
class SoGroup;

class PRDSystemController : public VP1Controller {

  Q_OBJECT

public:

  PRDSystemController(IVP1System * sys);
  virtual ~PRDSystemController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  //Use by the system to access the collection widget, which again is
  //the main entry point for communication with the prd collections
  //(loading/hiding/etc.).
  VP1CollectionWidget * collWidget() const;

  SoGroup * drawOptions(PRDDetType::Type) const;

  SoMaterial * getHighLightMaterial();
  double highLightMaterialWeight() const;//value indicates relative weight
				   //of highlight material. 0.0
				   //obviously means no high light
				   //material, and values > 999.0
				   //should be taken to mean infinite
				   //weight

  //Stateless on-click info:
  bool printInfoOnClick() const;
  bool zoomOnClick() const;
  bool muonOrientToChambersOnClick() const;

  //Access relevant information of controller:
  PRDCollHandleBase::COLOURMETHOD colourMethod() const;

  bool selectionModeMultiple() const;
  bool showSelectionLine() const;
  SoMaterial * getMultiSelectionLineMaterial() const;

  bool highLightOutliers() const;
  bool drawErrors() const;
  bool drawRDOs() const;

  bool projectSCTHits() const;

  //Cuts;
  VP1Interval cutAllowedEta() const;
  QList<VP1Interval> cutAllowedPhi() const;//All off: empty list. All on: list with one entry: ]-inf,inf[
  PRDCommonFlags::TrackerPartsFlags trackerPartsFlags() const;
  bool sctExcludeIsolatedClusters() const;
  //Used ID parts:
//   InDetProjFlags::DetTypeFlags inDetPartsUsingProjections() const;

  std::set<PRDDetType::Type> shownCollectionTypes() const;

signals:
  //The following signals are emitted when any of the relevant information in the controller changes value:

  //Display modes and projections:
  //  void generalPRDDetailLevelChanged(PRDCollHandleBase::DETAIL);
  void colourMethodChanged(PRDCollHandleBase::COLOURMETHOD);
  void selectionModeMultipleChanged(bool);
  void showSelectionLineChanged(bool);
  void clearSelection() const;//only signal
  void highLightMaterialWeightChanged(const double&);
  void highLightOutliersChanged(bool);
  void drawErrorsChanged(bool);
  void drawRDOsChanged(bool);
//   void projectSCTHitsChanged(bool);

  //Cuts:
  void cutAllowedEtaChanged(const VP1Interval&);
  void cutAllowedPhiChanged(const QList<VP1Interval>&);
  void trackerPartsFlagsChanged(PRDCommonFlags::TrackerPartsFlags);
  void sctExcludeIsolatedClustersChanged(bool);

  //Used ID parts:
//   void inDetPartsUsingProjectionsChanged(InDetProjFlags::DetTypeFlags);
  void shownCollectionTypesChanged(const std::set<PRDDetType::Type>&);
  
private:

  class Imp;
  Imp * m_d;

  //For verbose output:
  template <class T> static QString toString( const T& t ) { return VP1Controller::toString(t); }//unhide base methods

  static QString toString( const PRDCollHandleBase::DETAIL& par ) { return PRDCollHandleBase::toString(par); }
  static QString toString( const PRDCollHandleBase::COLOURMETHOD& par ) { return PRDCollHandleBase::toString(par); }
  static QString toString( const std::set<PRDDetType::Type>& s ) { return "["+str(s.size())+" types]"; }


private slots:
  void emitClearSelection();
  // void updateHighlightGui();
  void possibleChange_colourMethod();
  void possibleChange_selectionModeMultiple();
  void possibleChange_showSelectionLine();
  void possibleChange_highLightMaterialWeight();
  void possibleChange_highLightOutliers();
  void possibleChange_drawErrors();
  void possibleChange_drawRDOs();
  void possibleChange_cutAllowedEta();
  void possibleChange_cutAllowedPhi();
  void possibleChange_trackerPartsFlags();
  void possibleChange_sctExcludeIsolatedClusters();
  
//   void possibleChange_projectSCTHits();
//   void possibleChange_inDetPartsUsingProjections();
  void possibleChange_shownCollectionTypes();

};

#endif
