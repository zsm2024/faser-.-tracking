/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FAsER collaborations
*/

/** @file IWaveformDigiTool.h Interface file for WaveformDigiTool.
 * 
 * Provides times and offsets (in ns) for different channels in the
 * waveform digitizer.  This aligns the input signals for different
 * path lengths and cable delays.
 *
 */

// Multiple inclusion protection
#ifndef IWAVEFORMDIGICONDITIONSTOOL
#define IWAVEFORMDIGICONDITIONSTOOL

//STL includes
#include <map>

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"


class IWaveformDigiConditionsTool: virtual public IAlgTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~IWaveformDigiConditionsTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IWaveformDigiConditionsTool, 1, 0);

  // Methods to return digitization parameters

  // Baseline parameters
  virtual float base_mean(void) const = 0;
  virtual float base_mean(const EventContext& ctx) const = 0;

  virtual float base_rms(void) const = 0;
  virtual float base_rms(const EventContext& ctx) const = 0;

  // Crystal Ball parameters
  virtual float cb_mean(void) const = 0;
  virtual float cb_mean(const EventContext& ctx) const = 0;

  virtual float cb_sigma(void) const = 0;
  virtual float cb_sigma(const EventContext& ctx) const = 0;

  virtual float cb_norm(void) const = 0;
  virtual float cb_norm(const EventContext& ctx) const = 0;

  virtual float cb_alpha(void) const = 0;
  virtual float cb_alpha(const EventContext& ctx) const = 0;

  virtual float cb_n(void) const = 0;
  virtual float cb_n(const EventContext& ctx) const = 0;

  // Time dependence parameters
  virtual float preshower_time_slope(void) const = 0;
  virtual float preshower_time_slope(const EventContext& ctx) const = 0;

  virtual float trigger_time_slope(void) const = 0;
  virtual float trigger_time_slope(const EventContext& ctx) const = 0;

};

//---------------------------------------------------------------------- 
#endif // WAVEFORMDIGITOOL
