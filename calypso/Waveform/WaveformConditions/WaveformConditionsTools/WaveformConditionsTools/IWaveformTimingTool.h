/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FAsER collaborations
*/

/** @file IWaveformTimingTool.h Interface file for WaveformTimingTool.
 * 
 * Provides times and offsets (in ns) for different channels in the
 * waveform digitizer.  This aligns the input signals for different
 * path lengths and cable delays.
 *
 */

// Multiple inclusion protection
#ifndef IWAVEFORMTIMINGTOOL
#define IWAVEFORMTIMINGTOOL

//STL includes
#include <map>

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"


class IWaveformTimingTool: virtual public IAlgTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~IWaveformTimingTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IWaveformTimingTool, 1, 0);

  // Methods to return timing data

  // Nominal trigger time (in ns) in the digitizer readout
  virtual float nominalTriggerTime(void) const = 0;
  virtual float nominalTriggerTime(const EventContext& ctx) const = 0;

  // Channel-by-channel corrections to the nominal trigger time (in ns)
  // A given channel should be centered at nominal + offset
  virtual float triggerTimeOffset(int channel) const = 0;
  virtual float triggerTimeOffset(const EventContext& ctx, int channel) const = 0;

};

//---------------------------------------------------------------------- 
#endif // WAVEFORMTIMINGTOOL
