/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FAsER collaborations
*/

/** @file IWaveformRangeTool.h Interface file for WaveformRangeTool.
 */

// Multiple inclusion protection
#ifndef IWAVEFORMRANGETOOL
#define IWAVEFORMRANGETOOL

//STL includes
#include <map>

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

// ADC range in volts indexed by digitizer channel number
typedef std::map<int, float> WaveformRangeMap;

class IWaveformRangeTool: virtual public IAlgTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~IWaveformRangeTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IWaveformRangeTool, 1, 0);

  // Methods to return cable-mapping data 
  // Key is digitizer channel, pair is <type, identifier>
  virtual WaveformRangeMap getRangeMapping(const EventContext& ctx) const = 0;
  virtual WaveformRangeMap getRangeMapping(void) const = 0;

};

//---------------------------------------------------------------------- 
#endif // WAVEFORMRANGETOOL
