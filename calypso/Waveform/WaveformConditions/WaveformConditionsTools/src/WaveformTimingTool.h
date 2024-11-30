// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and CERN collaborations
*/

/** @file WaveformTimingTool.h Header file for WaveformTimingTool.
    @author Eric Torrence, 20/04/22
*/

// Multiple inclusion protection
#ifndef WAVEFORM_TIMING_TOOL
#define WAVEFORM_TIMING_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveformConditionsTools/IWaveformTimingTool.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

/** This class contains a Tool that reads Waveform timing data and makes it available 
    to other algorithms. 
*/

class WaveformTimingTool: public extends<AthAlgTool, IWaveformTimingTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  WaveformTimingTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~WaveformTimingTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // Methods to return timing data
  // Channels indexed by digitizer channel number
  // All times are in ns

  // Nominal trigger time (in ns) in the digitizer readout
  virtual float nominalTriggerTime(void) const override;
  virtual float nominalTriggerTime(const EventContext& ctx) const override;

  // Channel-by-channel corrections to the nominal trigger time (in ns)
  // A given channel should be centered at nominal + offset
  virtual float triggerTimeOffset(int channel) const override;
  virtual float triggerTimeOffset(const EventContext& ctx, int channel) const override;

 private:

  // Read Cond Handle
  SG::ReadCondHandleKey<AthenaAttributeList> m_timingReadKey{this, "TimingReadKey", "/WAVE/DAQ/Timing", "Key of timing folder"};
  SG::ReadCondHandleKey<CondAttrListCollection> m_offsetReadKey{this, "OffsetReadKey", "/WAVE/DAQ/TimingOffset", "Key of timing offset folder"};

  //ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

};

//---------------------------------------------------------------------- 
#endif // WAVEFORM_CABLE_MAPPING_TOOL
