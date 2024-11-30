/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Header file for class VP1FaserChannel                  //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: May 2007                              //
//                                                         //
/////////////////////////////////////////////////////////////

#ifndef VP1FASERCHANNEL_H
#define VP1FASERCHANNEL_H

#include "VP1Base/IVP13DStandardChannelWidget.h"

class VP1FaserChannel : public IVP13DStandardChannelWidget {

  Q_OBJECT

public:

  VP1FaserChannel();
  void init();
  virtual ~VP1FaserChannel(){}

};

#endif
