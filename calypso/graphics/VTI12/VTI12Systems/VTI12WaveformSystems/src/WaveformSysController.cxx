/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Implementation of class WaveformSysController             //
//                                                            //
//  Author: Dave Casper (dcasper@uci.edu)                     //
//  Initial version: August 2021                              //
//                                                            //
////////////////////////////////////////////////////////////////

#define VP1IMPVARNAME m_d

#include "VTI12WaveformSystems/WaveformSysController.h"
#include "ui_waveformcontrollerform.h"

#include "VP1Base/VP1Serialise.h"
#include "VP1Base/VP1Deserialise.h"

// System of units
#ifdef BUILDVP1LIGHT
	#include "GeoModelKernel/Units.h"
	#define SYSTEM_OF_UNITS GeoModelKernelUnits // --> 'GeoModelKernelUnits::cm'
#else
  #include "GaudiKernel/SystemOfUnits.h"
  #define SYSTEM_OF_UNITS Gaudi::Units // --> 'Gaudi::Units::cm'
#endif

#include <cmath>

//____________________________________________________________________
class WaveformSysController::Imp {
public:
  WaveformSysController * theclass;
  Ui::VP1WaveformControllerForm ui;

  static SbColor4f color4f(const QColor& col, int transp_int) {
    return SbColor4f(std::max<float>(0.0f,std::min<float>(1.0f,col.redF())),
		     std::max<float>(0.0f,std::min<float>(1.0f,col.greenF())),
		     std::max<float>(0.0f,std::min<float>(1.0f,col.blueF())),
		     std::max<float>(0.0f,std::min<float>(1.0f,1.0-transp_int/100.0)));
  }

  bool last_vetoEnabled;
  bool last_vetoNuEnabled;
  bool last_triggerEnabled;
  bool last_preshowerEnabled;
  bool last_calorimeterEnabled;
  bool last_clockEnabled;
  bool last_testEnabled;
};

//____________________________________________________________________
WaveformSysController::WaveformSysController(IVP1System * sys)
  : VP1Controller(sys,"WaveformSysController"), m_d(new Imp)
{
  m_d->theclass = this;
  m_d->ui.setupUi(this);

  addUpdateSlot(SLOT(possibleChange_vetoEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_vetoEnabled);

  addUpdateSlot(SLOT(possibleChange_vetoNuEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_vetoNuEnabled);

  addUpdateSlot(SLOT(possibleChange_triggerEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_triggerEnabled);

  addUpdateSlot(SLOT(possibleChange_preshowerEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_preshowerEnabled);

  addUpdateSlot(SLOT(possibleChange_calorimeterEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_calorimeterEnabled);

  addUpdateSlot(SLOT(possibleChange_clockEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_clockEnabled);

  addUpdateSlot(SLOT(possibleChange_testEnabled()));
  connectToLastUpdateSlot(m_d->ui.checkBox_testEnabled);

  initLastVars();
}

//____________________________________________________________________
WaveformSysController::~WaveformSysController()
{
  delete m_d;
}

bool WaveformSysController::vetoEnabled() const
{
  return m_d->ui.checkBox_vetoEnabled->isChecked();
}

bool WaveformSysController::vetoNuEnabled() const
{
  return m_d->ui.checkBox_vetoNuEnabled->isChecked();
}

bool WaveformSysController::triggerEnabled() const
{
  return m_d->ui.checkBox_triggerEnabled->isChecked();
}

bool WaveformSysController::preshowerEnabled() const
{
  return m_d->ui.checkBox_preshowerEnabled->isChecked();
}

bool WaveformSysController::calorimeterEnabled() const
{
  return m_d->ui.checkBox_calorimeterEnabled->isChecked();
}

bool WaveformSysController::clockEnabled() const
{
  return m_d->ui.checkBox_clockEnabled->isChecked();
}

bool WaveformSysController::testEnabled() const
{
  return m_d->ui.checkBox_testEnabled->isChecked();
}

int WaveformSysController::currentSettingsVersion() const
{
  return 0;
}

//____________________________________________________________________
void WaveformSysController::actualSaveSettings(VP1Serialise&s) const
{

  s.save(m_d->ui.checkBox_vetoEnabled);
  s.save(m_d->ui.checkBox_vetoNuEnabled);
  s.save(m_d->ui.checkBox_triggerEnabled);
  s.save(m_d->ui.checkBox_preshowerEnabled);
  s.save(m_d->ui.checkBox_calorimeterEnabled);
  s.save(m_d->ui.checkBox_clockEnabled);
  s.save(m_d->ui.checkBox_testEnabled);

}

//____________________________________________________________________
void WaveformSysController::actualRestoreSettings(VP1Deserialise& s)
{
  if (s.version() != 0) {
    message("Warning: State data in .vp1 file has unsupported version ("+str(s.version())+")");
    return;
  }

  s.restore(m_d->ui.checkBox_vetoEnabled);
  s.restore(m_d->ui.checkBox_vetoNuEnabled);
  s.restore(m_d->ui.checkBox_triggerEnabled);
  s.restore(m_d->ui.checkBox_preshowerEnabled);
  s.restore(m_d->ui.checkBox_calorimeterEnabled);
  s.restore(m_d->ui.checkBox_clockEnabled);
  s.restore(m_d->ui.checkBox_testEnabled);

}

///////////////////////////////////////////////////////////////////////////
// Test for possible changes in values and emit signals as appropriate:
// (possibleChange_XXX() slots code provided by macros)
#define VP1CONTROLLERCLASSNAME WaveformSysController
#include "VP1Base/VP1ControllerMacros.h"
POSSIBLECHANGE_IMP(vetoEnabled)
POSSIBLECHANGE_IMP(vetoNuEnabled)
POSSIBLECHANGE_IMP(triggerEnabled)
POSSIBLECHANGE_IMP(preshowerEnabled)
POSSIBLECHANGE_IMP(calorimeterEnabled)
POSSIBLECHANGE_IMP(clockEnabled)
POSSIBLECHANGE_IMP(testEnabled)
