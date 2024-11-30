/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


////////////////////////////////////////////////////////////////
//                                                            //
//  Header file for class WaveformSysController               //
//                                                            //
//  Description: Waveform system controller.                  //
//                                                            //
//  Author: Dave Casper (dcasper@uci.edu)                     //
//  Initial version: August 2021                              //
//                                                            //
////////////////////////////////////////////////////////////////

#ifndef WAVEFORMSYSCONTROLLER_H
#define WAVEFORMSYSCONTROLLER_H

#include "VP1Base/VP1Controller.h"
#include <Inventor/C/errors/debugerror.h>
#include <Inventor/SbColor4f.h>
class SoMaterial;

class WaveformSysController : public VP1Controller {

  Q_OBJECT

public:

  WaveformSysController(IVP1System * sys);
  virtual ~WaveformSysController();

  void actualRestoreSettings(VP1Deserialise&);
  int currentSettingsVersion() const;
  void actualSaveSettings(VP1Serialise&) const;

  ///////////////////////////////////
  //  Access Methods for settings  //
  ///////////////////////////////////

  bool vetoEnabled() const;
  bool vetoNuEnabled() const;
  bool triggerEnabled() const;
  bool preshowerEnabled() const;
  bool calorimeterEnabled() const;
  bool clockEnabled() const;
  bool testEnabled() const;


  ///////////////////////////////////////
  //  Signals for changes in settings  //
  ///////////////////////////////////////
signals:
  void vetoEnabledChanged(bool);
  void vetoNuEnabledChanged(bool);
  void triggerEnabledChanged(bool);
  void preshowerEnabledChanged(bool);
  void calorimeterEnabledChanged(bool);
  void clockEnabledChanged(bool);
  void testEnabledChanged(bool);

private:

  class Imp;
  Imp * m_d;

private slots:
  void possibleChange_vetoEnabled();
  void possibleChange_vetoNuEnabled();
  void possibleChange_triggerEnabled();
  void possibleChange_preshowerEnabled();
  void possibleChange_calorimeterEnabled();
  void possibleChange_clockEnabled();
  void possibleChange_testEnabled();

};


#endif
