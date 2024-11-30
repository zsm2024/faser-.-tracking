/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WAVERAWEVENT_RAWWAVEFORMCONTAINER_H
#define WAVERAWEVENT_RAWWAVEFORMCONTAINER_H

#include "WaveRawEvent/RawWaveform.h"
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"

// Make this a class in case we need to add some functions
class RawWaveformContainer : public DataVector<RawWaveform> {
 public:
  void print() const;
};

std::ostream 
&operator<<(std::ostream &out, const RawWaveformContainer &container);

CLASS_DEF(RawWaveformContainer, 1275584332, 1 )
SG_BASE(RawWaveformContainer, DataVector<RawWaveform>);

#endif // WAVERAWEVENT_RAWWAVEFORMCONTAINER_H
