// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and CERN collaborations
*/

/** @file WaveformCableMappingTool.h Header file for WaveformCableMappingTool.
    @author Eric Torrence, 05/02/22
    Based on FaserSCT_CableMappingTool
*/

// Multiple inclusion protection
#ifndef WAVEFORM_CABLE_MAPPING_TOOL
#define WAVEFORM_CABLE_MAPPING_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveformConditionsTools/IWaveformCableMappingTool.h"

// Identifiers
#include "Identifier/Identifier.h"
#include "FaserCaloIdentifier/EcalID.h"
#include "ScintIdentifier/VetoID.h"
#include "ScintIdentifier/VetoNuID.h"
#include "ScintIdentifier/TriggerID.h"
#include "ScintIdentifier/PreshowerID.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

/** This class contains a Tool that reads Waveform cable mapping data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class WaveformCableMappingTool: public extends<AthAlgTool, IWaveformCableMappingTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  WaveformCableMappingTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~WaveformCableMappingTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // Methods to return calibration data
  // Map indexed by digitizer channel number
  // Returns detector type "calo", "trigger", "veto", "preshower" and identifier 
  virtual WaveformCableMap getCableMapping(const EventContext& ctx) const override;
  virtual WaveformCableMap getCableMapping(void) const override;

  // Reverse mapping, reads idenfifier and returns digitizer channel
  // Returns -1 if match not found for given identifier
  virtual int getChannelMapping(const EventContext& ctx, const Identifier id) const override;
  virtual int getChannelMapping(const Identifier id) const override;

 private:
  // Read Cond Handle
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/WAVE/DAQ/CableMapping", "Key of input cabling folder"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

  // ID helpers
  const EcalID* m_ecalID{nullptr};
  const VetoID* m_vetoID{nullptr};
  const VetoNuID* m_vetoNuID{nullptr};
  const TriggerID* m_triggerID{nullptr};
  const PreshowerID* m_preshowerID{nullptr};


};

//---------------------------------------------------------------------- 
#endif // WAVEFORM_CABLE_MAPPING_TOOL
