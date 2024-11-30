/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
//Autogenerated VP1 Factory Code Implementation File (Fri Aug 19 13:36:52 CEST 2016)

#include <QtPlugin>
#include "VTI12WaveformPlugin/VP1WaveformPlugin_VP1AutoFactory.h"
#include "VTI12WaveformPlugin/VP1WaveformChannel.h"

QStringList VP1WaveformPlugin_VP1AutoFactory::channelWidgetsProvided() const
{
  return QStringList()
        << "Waveform"
         ;
}

IVP1ChannelWidget * VP1WaveformPlugin_VP1AutoFactory::getChannelWidget(const QString & channelwidget)
{
  if (channelwidget == "Waveform")
    return new VP1WaveformChannel();

  return 0;
}

