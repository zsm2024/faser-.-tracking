/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1WaveformChannel             //
//                                                         //
//  Author: Dave Casper <dcasper@uci.edu>                  //
//                                                         //
//  Initial version: August 2021                           //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VTI12WaveformPlugin/VP1WaveformChannel.h"
#include "VTI12WaveformSystems/VP1WaveformSystem.h"
#include "VTI12WaveformSystems/VP1WaveformHitSystem.h"
#include "VP1Base/VP1QtUtils.h"

VP1WaveformChannel::VP1WaveformChannel()
  : IVP12DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1WaveformChannel,"Waveform"),
				"This channel shows the FASER waveform data.",
				"dcasper@uci.edu")
{
}

void VP1WaveformChannel::init()
{
  addSystem(new VP1WaveformSystem);
  addSystem(new VP1WaveformHitSystem);
  // addSystem(new VP1GeometrySystem);
  // if (VP1QtUtils::environmentVariableIsOn("VP1_SECOND_GEOMSYS"))
  //   addSystem(new VP1GeometrySystem(VP1GeoFlags::None,"AltGeo"),IVP13DStandardChannelWidget::StartDisabled);
}
