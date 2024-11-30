/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS and FASER collaborations
*/

////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class PRDSystemController               //
//                                                            //
//  Author: Thomas H. Kittelmann (Thomas.Kittelmann@cern.ch)  //
//  Initial version: November 2007                            //
//                                                            //
////////////////////////////////////////////////////////////////

#define VP1IMPVARNAME m_d

#include "VTI12PRDSystems/PRDSystemController.h"
#include "ui_vp1prdcontrollerform.h"
#include "ui_prd_settings_cuts_form.h"
#include "ui_prd_settings_display_form.h"
#include "ui_prd_settings_interactions_form.h"
#include "VP1Base/VP1QtInventorUtils.h"
#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"
#include "VP1Base/VP1QtUtils.h"
#include "VP1Base/VP1CollectionWidget.h"
#include <Inventor/nodes/SoMaterial.h>
#include <QBuffer>
#include <QTimer>
#include <QSet>

//____________________________________________________________________
class PRDSystemController::Imp {
public:
  PRDSystemController * theclass = nullptr;
  Ui::VP1PrdControllerForm ui{};
  Ui::PRDSysSettingsInteractionsForm ui_int{};
  Ui::PRDSysSettingsDisplayForm ui_disp{};
  Ui::PRDSysSettingsCutsForm ui_cuts{};
  VP1CollectionWidget * collWidget = nullptr;

  SoMaterial * highlightmaterial = nullptr;

  PRDCollHandleBase::COLOURMETHOD last_colourMethod;
  bool last_drawErrors = false;
  bool last_drawRDOs = false;
  bool last_highLightOutliers = false;
  double last_highLightMaterialWeight = 0.0;
  bool last_projectSCTHits = false;
  //Cuts:
  VP1Interval last_cutAllowedEta;
  QList<VP1Interval> last_cutAllowedPhi;//All off: empty list. All on: list with one entry: ]-inf,inf[
  PRDCommonFlags::TrackerPartsFlags last_trackerPartsFlags;
  bool last_sctExcludeIsolatedClusters = false;
  std::set<PRDDetType::Type> last_shownCollectionTypes;
  bool last_selectionModeMultiple = false;
  bool last_showSelectionLine = false;
  //Used ID parts:
//   InDetProjFlags::DetTypeFlags last_inDetPartsUsingProjections;

  QSet<PRDDetType::Type> shownCollectionTypes() const;
};

//____________________________________________________________________
PRDSystemController::PRDSystemController(IVP1System * sys)
  : VP1Controller(sys,"PRDSystemController"),m_d(new Imp)
{
  m_d->theclass = this;
  m_d->ui.setupUi(this);

  //FIXME: Dialogs and collwidget!
  m_d->collWidget = new VP1CollectionWidget;
  //   m_d->trackcollwidget = new TrackCollWidget;
  setupCollWidgetInScrollArea(m_d->ui.collWidgetScrollArea,m_d->collWidget);
  initDialog(m_d->ui_cuts, m_d->ui.pushButton_settings_cuts);
  initDialog(m_d->ui_disp, m_d->ui.pushButton_settings_display);
  initDialog(m_d->ui_int, m_d->ui.pushButton_settings_interactions);

  m_d->highlightmaterial = 0;

  m_d->ui_int.matButton_multiselline->setMaterial(VP1MaterialButton::createMaterial(1,1,0));

  m_d->ui_cuts.groupBox_cuts_tracksegmentassociation->setVisible(false);//Since it is not used at the moment!
  if (!VP1QtUtils::environmentVariableIsOn("VP1_DEVEL_ENABLEREFIT"))
    m_d->ui_int.groupBox_selectionMode->setVisible(false);

  m_d->ui_disp.widget_drawOptions_PixelSCT->setComplexityDisabled();
  m_d->ui_disp.widget_drawOptions_PixelSCT->setPointSizes(3.0);

  m_d->ui_cuts.etaPhiCutWidget->setEtaCutEnabled(false);

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //  Setup connections which monitor changes in the controller so that we may emit signals as appropriate:  //
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

  addUpdateSlot(SLOT(possibleChange_colourMethod()));
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_colourBySegment);
  
  addUpdateSlot(SLOT(possibleChange_drawErrors()));
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_drawErrors);
  
  addUpdateSlot(SLOT(possibleChange_drawRDOs()));
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_drawRDOs);
  
  addUpdateSlot(SLOT(possibleChange_highLightOutliers()));
  connectToLastUpdateSlot(m_d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightMaterialWeight()));
  connectToLastUpdateSlot(m_d->ui_disp.horizontalSlider_highlights_intensity);

  addUpdateSlot(SLOT(possibleChange_highLightOutliers()));
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_highlight_trackoutliers);
  connectToLastUpdateSlot(m_d->ui_disp.checkBox_colourByTrack);
  connectToLastUpdateSlot(m_d->ui_disp.horizontalSlider_highlights_intensity);

//   addUpdateSlot(SLOT(possibleChange_projectSCTHits()));
//   connectToLastUpdateSlot(m_d->ui_disp.checkBox_showsct_projected);

  addUpdateSlot(SLOT(possibleChange_cutAllowedEta()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedEtaChanged(const VP1Interval&)));

  addUpdateSlot(SLOT(possibleChange_cutAllowedPhi()));
  connectToLastUpdateSlot(m_d->ui_cuts.etaPhiCutWidget,SIGNAL(allowedPhiChanged(const QList<VP1Interval>&)));

  addUpdateSlot(SLOT(possibleChange_trackerPartsFlags()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_tracker_interface);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_tracker_upstream);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_tracker_central);
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_tracker_downstream);

  addUpdateSlot(SLOT(possibleChange_sctExcludeIsolatedClusters()));
  connectToLastUpdateSlot(m_d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters);

  // addUpdateSlot(SLOT(possibleChange_inDetPartsUsingProjections()));
  // connectToLastUpdateSlot(this,SIGNAL(projectSCTHitsChanged(bool)));
  // connectToLastUpdateSlot(this,SIGNAL(trackerPartsFlagsChanged(PRDCommonFlags::TrackerPartsFlags)));
  // connectToLastUpdateSlot(collWidget(),SIGNAL(visibleStdCollectionTypesChanged(const QList<qint32>&)));

  addUpdateSlot(SLOT(possibleChange_selectionModeMultiple()));
  connectToLastUpdateSlot(m_d->ui_int.checkBox_selModeMultiple);

  addUpdateSlot(SLOT(possibleChange_showSelectionLine()));
  connectToLastUpdateSlot(m_d->ui_int.checkBox_showSelectionLine);

  addUpdateSlot(SLOT(possibleChange_shownCollectionTypes()));
  connectToLastUpdateSlot(collWidget(),SIGNAL(visibleStdCollectionTypesChanged(const QList<qint32>&)));

  // addUpdateSlot(SLOT(updateHighlightGui()));
  // connectToLastUpdateSlot(this,SIGNAL(highLightOutliersChanged(bool)));

  //Stateless:
  connect(m_d->ui_int.pushButton_clearSelection,SIGNAL(clicked()),this,SLOT(emitClearSelection()));

  initLastVars();

}

//____________________________________________________________________
PRDSystemController::~PRDSystemController()
{
  if (m_d->highlightmaterial)
    m_d->highlightmaterial->unref();
  delete m_d;
}

//____________________________________________________________________
VP1CollectionWidget * PRDSystemController::collWidget() const
{
  return m_d->collWidget;
}

//____________________________________________________________________
SoGroup * PRDSystemController::drawOptions(PRDDetType::Type t) const
{
  //FIXME: SET APPROPRIATE DEFAULTS!!!!
  switch (t) {
  case PRDDetType::SCT:
  case PRDDetType::SpacePoints:
    return m_d->ui_disp.widget_drawOptions_PixelSCT->drawOptionsGroup();
  default:
    return m_d->ui_disp.widget_drawOptions_PixelSCT->drawOptionsGroup();
  }
}

//____________________________________________________________________
QSet<PRDDetType::Type> PRDSystemController::Imp::shownCollectionTypes() const
{
  QSet<PRDDetType::Type> s;
  foreach (qint32 i,collWidget->visibleStdCollectionTypes()) {
    bool ok;
    PRDDetType::Type t = PRDDetType::intToType(i,ok);
    if (ok)
      s.insert(t);
  }
  return s;
}

//____________________________________________________________________
SoMaterial * PRDSystemController::getMultiSelectionLineMaterial() const
{
  QList<SoMaterial*> mats = m_d->ui_int.matButton_multiselline->handledMaterials();
  if (mats.count()!=1) {
    message("ERROR: No material in button!");
    return 0;//probably giving a crash...
  }
  return mats.at(0);
}

//____________________________________________________________________
void PRDSystemController::emitClearSelection()
{
  messageVerbose("Emitting clearSelection()");
  emit clearSelection();
}

//____________________________________________________________________
SoMaterial * PRDSystemController::getHighLightMaterial()
{
  if (!m_d->highlightmaterial) {
    m_d->highlightmaterial = new SoMaterial;
    m_d->highlightmaterial->ref();
    m_d->highlightmaterial->diffuseColor.setValue( 1.0f, 0.0f, 0.0f );
    m_d->ui_disp.materialbutton_highlights_material->copyValuesFromMaterial(m_d->highlightmaterial);
    m_d->ui_disp.materialbutton_highlights_material->handleMaterial(m_d->highlightmaterial);
  }
  return m_d->highlightmaterial;
}

//____________________________________________________________________
bool PRDSystemController::printInfoOnClick() const
{
  return m_d->ui_int.checkBox_printinfo->isChecked();
}

//____________________________________________________________________
bool PRDSystemController::zoomOnClick() const
{
  return m_d->ui_int.checkBox_zoom->isChecked();
}

//____________________________________________________________________
VP1Interval PRDSystemController::cutAllowedEta() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedEta();
}

//____________________________________________________________________
QList<VP1Interval> PRDSystemController::cutAllowedPhi() const
{
  return m_d->ui_cuts.etaPhiCutWidget->allowedPhi();
}

//____________________________________________________________________
PRDCommonFlags::TrackerPartsFlags PRDSystemController::trackerPartsFlags() const
{
  PRDCommonFlags::TrackerPartsFlags flag = PRDCommonFlags::None;
  if (m_d->ui_cuts.checkBox_cut_tracker_interface->isChecked()) flag |= PRDCommonFlags::Interface;
  if (m_d->ui_cuts.checkBox_cut_tracker_upstream->isChecked()) flag |= PRDCommonFlags::Upstream;
  if (m_d->ui_cuts.checkBox_cut_tracker_central->isChecked()) flag |= PRDCommonFlags::Central;
  if (m_d->ui_cuts.checkBox_cut_tracker_downstream->isChecked()) flag |= PRDCommonFlags::Downstream;
  return flag;
}

//____________________________________________________________________
bool PRDSystemController::sctExcludeIsolatedClusters() const
{
  return m_d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters->isChecked();
}

//____________________________________________________________________
PRDCollHandleBase::COLOURMETHOD PRDSystemController::colourMethod() const
{
  if (m_d->ui_disp.checkBox_colourByTrack->isChecked()) {
    if (m_d->ui_disp.checkBox_colourBySegment->isChecked())
      return PRDCollHandleBase::BySegmentAndTrack;
    else
      return PRDCollHandleBase::ByTrack;
  } else {
    if (m_d->ui_disp.checkBox_colourBySegment->isChecked())
      return PRDCollHandleBase::BySegment;
    else
      return PRDCollHandleBase::ByTechOnly;
  }
}

//____________________________________________________________________
bool PRDSystemController::drawErrors() const
{
  if (m_d->ui_disp.checkBox_drawErrors->isChecked()) return true;
  return false;
}

//____________________________________________________________________
bool PRDSystemController::drawRDOs() const
{
  if (m_d->ui_disp.checkBox_drawRDOs->isChecked()) return true;
  return false;
}


//____________________________________________________________________
bool PRDSystemController::highLightOutliers() const
{
  return highLightMaterialWeight() != 0.0
    && m_d->ui_disp.checkBox_highlight_trackoutliers->isChecked() && m_d->ui_disp.checkBox_colourByTrack->isChecked();
}

// //____________________________________________________________________
// void PRDSystemController::updateHighlightGui()
// {
//   const bool outlierHLEnabledInGui(m_d->ui_disp.checkBox_highlight_trackoutliers->isChecked() && m_d->ui_disp.checkBox_colourByTrack->isChecked());
//   m_d->ui_disp.checkBox_highlight_trtht->setEnabled(!outlierHLEnabledInGui);
//   m_d->ui_disp.checkBox_highlight_maskedmdts->setEnabled(!outlierHLEnabledInGui);
//   m_d->ui_disp.checkBox_highlight_mdt_ADC_below->setEnabled(!outlierHLEnabledInGui);
//   m_d->ui_disp.spinBox_highlight_mdt_ADC_below->setEnabled(!outlierHLEnabledInGui);
// }

//____________________________________________________________________
double PRDSystemController::highLightMaterialWeight() const
{
  int val = m_d->ui_disp.horizontalSlider_highlights_intensity->value();
  if (val<=1) return 0.0;
  if (val==2) return 1.0;
  if (val==3) return 1.9;
  if (val==4) return 2.8;
  if (val==5) return 5.0;
  return 99999.0;
}

//____________________________________________________________________
// bool PRDSystemController::projectSCTHits() const { return m_d->ui_disp.checkBox_showsct_projected->isChecked(); }

// //____________________________________________________________________
// InDetProjFlags::DetTypeFlags PRDSystemController::inDetPartsUsingProjections() const
// {
//   std::set<PRDDetType::Type> showncolltypes =  shownCollectionTypes();
//   PRDCommonFlags::InDetPartsFlags indetparts = inDetPartsFlags();

//   InDetProjFlags::DetTypeFlags flag = InDetProjFlags::NoDet;

//   if ( projectPixelHits() && showncolltypes.find(PRDDetType::Pixel)!=showncolltypes.end() ) {
//     if (indetparts & PRDCommonFlags::BarrelPositive)
//       flag |= InDetProjFlags::Pixel_brlpos;
//     if (indetparts & PRDCommonFlags::BarrelNegative)
//       flag |= InDetProjFlags::Pixel_brlneg;
//     if (indetparts & PRDCommonFlags::EndCapPositive)
//       flag |= InDetProjFlags::Pixel_ecpos;
//     if (indetparts & PRDCommonFlags::EndCapNegative)
//       flag |= InDetProjFlags::Pixel_ecneg;
//   }

//   if ( projectSCTHits() && showncolltypes.find(PRDDetType::SCT)!=showncolltypes.end() ) {
//     if (indetparts & PRDCommonFlags::BarrelPositive)
//       flag |= InDetProjFlags::SCT_brlpos;
//     if (indetparts & PRDCommonFlags::BarrelNegative)
//       flag |= InDetProjFlags::SCT_brlneg;
//     if (indetparts & PRDCommonFlags::EndCapPositive)
//       flag |= InDetProjFlags::SCT_ecpos;
//     if (indetparts & PRDCommonFlags::EndCapNegative)
//       flag |= InDetProjFlags::SCT_ecneg;
//   }

//   if ( projectTRTHits() && showncolltypes.find(PRDDetType::TRT)!=showncolltypes.end() ) {
//      if (indetparts & PRDCommonFlags::BarrelPositive)
//       flag |= InDetProjFlags::TRT_brlpos;
//     if (indetparts & PRDCommonFlags::BarrelNegative)
//       flag |= InDetProjFlags::TRT_brlneg;
//     if (indetparts & PRDCommonFlags::EndCapPositive)
//       flag |= InDetProjFlags::TRT_ecpos;
//     if (indetparts & PRDCommonFlags::EndCapNegative)
//       flag |= InDetProjFlags::TRT_ecneg;
//   }

//   return flag;
// }

//____________________________________________________________________
bool PRDSystemController::selectionModeMultiple() const
{
  return m_d->ui_int.checkBox_selModeMultiple->isChecked();
}

//____________________________________________________________________
bool PRDSystemController::showSelectionLine() const
{
  return m_d->ui_int.checkBox_showSelectionLine->isChecked();
}

//____________________________________________________________________
std::set<PRDDetType::Type> PRDSystemController::shownCollectionTypes() const
{
  std::set<PRDDetType::Type> s;
  foreach(qint32 i, collWidget()->visibleStdCollectionTypes()) {
    bool ok;
    PRDDetType::Type t = PRDDetType::intToType(i, ok);
    if (ok)
      s.insert(t);
    else
      message("shownCollectionTypes ERROR: Could not decode collection type");
  }
  return s;
}


//____________________________________________________________________
int PRDSystemController::currentSettingsVersion() const
{
  return 1;
}

//____________________________________________________________________
void PRDSystemController::actualSaveSettings(VP1Serialise&s) const
{
  s.save(m_d->ui_disp.checkBox_colourByTrack);
  s.save(m_d->ui_disp.checkBox_colourBySegment);
  s.save(m_d->ui_disp.checkBox_highlight_trackoutliers);
  s.save(m_d->ui_cuts.checkBox_cut_tracker_interface);
  s.save(m_d->ui_cuts.checkBox_cut_tracker_upstream);
  s.save(m_d->ui_cuts.checkBox_cut_tracker_central);
  s.save(m_d->ui_cuts.checkBox_cut_tracker_downstream);
  s.save(m_d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters);
//   s.save(m_d->ui_disp.checkBox_showsct_projected);
  s.save(m_d->ui_disp.horizontalSlider_highlights_intensity);
  s.save(m_d->ui_disp.materialbutton_highlights_material);
  s.save(m_d->ui_int.checkBox_selModeMultiple);
  s.save(m_d->ui_int.checkBox_showSelectionLine);
  s.save(m_d->ui_int.matButton_multiselline);
  s.save(m_d->ui_cuts.etaPhiCutWidget);
  s.save(m_d->ui_disp.widget_drawOptions_PixelSCT);
  s.save(m_d->ui_int.checkBox_printinfo);
  s.save(m_d->ui_int.checkBox_zoom);

  s.save(m_d->ui_disp.checkBox_drawErrors);
  s.save(m_d->ui_disp.checkBox_drawRDOs);

  //Not used yet:
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_track);
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_hits_not_on_track);
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_segments);
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_not_on_segments);

}


//____________________________________________________________________
void PRDSystemController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version()<0||s.version()>1) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }
  s.restore(m_d->ui_disp.checkBox_colourByTrack);
  s.restore(m_d->ui_disp.checkBox_colourBySegment);
  s.restore(m_d->ui_disp.checkBox_highlight_trackoutliers);
  s.restore(m_d->ui_cuts.checkBox_cut_tracker_interface);
  s.restore(m_d->ui_cuts.checkBox_cut_tracker_upstream);
  s.restore(m_d->ui_cuts.checkBox_cut_tracker_central);
  s.restore(m_d->ui_cuts.checkBox_cut_tracker_downstream);
  s.restore(m_d->ui_cuts.checkBox_cut_sct_excludeisolatedclusters);
//   s.restore(m_d->ui_disp.checkBox_showsct_projected);
  s.restore(m_d->ui_disp.horizontalSlider_highlights_intensity);
  s.restore(m_d->ui_disp.materialbutton_highlights_material);
  s.restore(m_d->ui_int.checkBox_selModeMultiple);
  s.restore(m_d->ui_int.checkBox_showSelectionLine);
  s.restore(m_d->ui_int.matButton_multiselline);
  s.restore(m_d->ui_cuts.etaPhiCutWidget);
  s.restore(m_d->ui_disp.widget_drawOptions_PixelSCT);
  s.restore(m_d->ui_int.checkBox_printinfo);
  s.restore(m_d->ui_int.checkBox_zoom);

  s.restore(m_d->ui_disp.checkBox_drawErrors);
  s.restore(m_d->ui_disp.checkBox_drawRDOs);
  
  //Not used yet:
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_track);
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_hits_not_on_track);
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_hits_on_segments);
  s.ignoreWidget(m_d->ui_cuts.checkBox_cuts_ts_exclude_not_on_segments);
}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME PRDSystemController
#include "VP1Base/VP1ControllerMacros.h"

//The actual code for each variable:
// POSSIBLECHANGE_IMP(projectSCTHits)
POSSIBLECHANGE_IMP(highLightMaterialWeight)
// POSSIBLECHANGE_IMP(inDetPartsUsingProjections)
POSSIBLECHANGE_IMP(cutAllowedEta)
POSSIBLECHANGE_IMP(cutAllowedPhi)
POSSIBLECHANGE_IMP(trackerPartsFlags)
POSSIBLECHANGE_IMP(sctExcludeIsolatedClusters)
POSSIBLECHANGE_IMP(shownCollectionTypes)
POSSIBLECHANGE_IMP(selectionModeMultiple)
POSSIBLECHANGE_IMP(showSelectionLine)
POSSIBLECHANGE_IMP(colourMethod)
POSSIBLECHANGE_IMP(highLightOutliers)
POSSIBLECHANGE_IMP(drawErrors)
POSSIBLECHANGE_IMP(drawRDOs)
