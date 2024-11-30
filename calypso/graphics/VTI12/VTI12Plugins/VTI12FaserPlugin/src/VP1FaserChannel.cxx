/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////
//                                                         //
//  Implementation of class VP1FaserChannel             //
//                                                         //
//  Author: Thomas Kittelmann <Thomas.Kittelmann@cern.ch>  //
//                                                         //
//  Initial version: May 2007                              //
//                                                         //
/////////////////////////////////////////////////////////////

#include "VTI12FaserPlugin/VP1FaserChannel.h"
#include "VTI12TrackSystems/VP1TrackSystem.h"
#include "VTI12GeometrySystems/VP1GeometrySystem.h"
#include "VTI12GuideLineSystems/VP1GuideLineSystem.h"
#include "VTI12SimHitSystems/VP1SimHitSystem.h"
#include "VTI12RawDataSystems/VP1RawDataSystem.h"
#include "VTI12PRDSystems/VP1PrepRawDataSystem.h"
#include "VP1Base/VP1QtUtils.h"

VP1FaserChannel::VP1FaserChannel()
  : IVP13DStandardChannelWidget(VP1CHANNELNAMEINPLUGIN(VP1FaserChannel,"Faser"),
				"This channel shows geometry, hits, clusters, spacepoints and tracks.",
				"Dave Casper dcasper@uci.edu")
{
}

void VP1FaserChannel::init()
{
  addSystem(new VP1TrackSystem);
  addSystem(new VP1PrepRawDataSystem);
  addSystem(new VP1RawDataSystem);
  addSystem(new VP1SimHitSystem);
  addSystem(new VP1GeometrySystem);
  addSystem(new VP1GuideLineSystem);
  // if (VP1QtUtils::environmentVariableIsOn("VP1_SECOND_GEOMSYS"))
  //   addSystem(new VP1GeometrySystem(VP1GeoFlags::None,"AltGeo"),IVP13DStandardChannelWidget::StartDisabled);
}
