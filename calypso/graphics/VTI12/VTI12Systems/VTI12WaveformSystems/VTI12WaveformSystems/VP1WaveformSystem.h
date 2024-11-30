/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1WAVEFORMSYSTEM_H
#define VP1WAVEFORMSYSTEM_H

/////////////////////////////////////////////////////////////////////////
//                                                                     //
//  Header file for class VP1WaveformSystem                            //
//                                                                     //
//  Author: Dave Casper <dcasper@uci.edu>                              //
//                                                                     //
//  Initial version: August 2021                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#include "VP1Base/IVP12DSystemSimple.h"
#include <QColor>//Fixme obsolete

class VP1WaveformSystem : public IVP12DSystemSimple {

  Q_OBJECT

public:

  ///////////////////
  //  Constructor  //
  ///////////////////

  VP1WaveformSystem();

  //////////////////////////////////////////
  //  Reimplementations from base class:  //
  //////////////////////////////////////////

  virtual ~VP1WaveformSystem();
  void buildPermanentItemCollection(StoreGateSvc* detstore, VP1GraphicsItemCollection *root);
  void buildEventItemCollection(StoreGateSvc*, VP1GraphicsItemCollection *root);
  void systemuncreate();
  QWidget * buildController();
  QByteArray saveState();
  void restoreFromState(QByteArray ba);

public slots:

  void updateVetoElements(bool);
  void updateVetoNuElements(bool);
  void updateTriggerElements(bool);
  void updatePreshowerElements(bool);
  void updateCalorimeterElements(bool);
  void updateClockElements(bool);
  void updateTestElements(bool);

private:
  class Imp;
  Imp * m_d;
};

#endif
