/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FAsER collaborations
*/

/** @file IWaveformCableMappingTool.h Interface file for WaveformCableMappingTool.
 */

// Multiple inclusion protection
#ifndef IWAVEFORMCABLEMAPPINGTOOL
#define IWAVEFORMCABLEMAPPINGTOOL

//STL includes
#include <map>

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include "Identifier/Identifier.h"

// Cable map indexed by digitizer channel number
// Contains detector type "calo", "trigger", "veto", "preshower" and identifier 
typedef std::map<int, std::pair<std::string, Identifier> > WaveformCableMap;

class IWaveformCableMappingTool: virtual public IAlgTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~IWaveformCableMappingTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(IWaveformCableMappingTool, 1, 0);

  // Methods to return cable-mapping data 
  // Key is digitizer channel, pair is <type, identifier>
  virtual WaveformCableMap getCableMapping(const EventContext& ctx) const = 0;
  virtual WaveformCableMap getCableMapping(void) const = 0;

  virtual int getChannelMapping(const EventContext& ctx, const Identifier id) const = 0;
  virtual int getChannelMapping(const Identifier id) const = 0;


};

//---------------------------------------------------------------------- 
#endif // WAVEFORMCABLEMAPPINGTOOL
