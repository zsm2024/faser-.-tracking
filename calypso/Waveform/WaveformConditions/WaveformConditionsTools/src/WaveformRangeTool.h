// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and CERN collaborations
*/

/** @file WaveformRangeTool.h Header file for WaveformRangeTool.
    @author Eric Torrence, 20/04/22
*/

// Multiple inclusion protection
#ifndef WAVEFORM_RANGE_TOOL
#define WAVEFORM_RANGE_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveformConditionsTools/IWaveformRangeTool.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

/** This class contains a Tool that reads Waveform range data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class WaveformRangeTool: public extends<AthAlgTool, IWaveformRangeTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  WaveformRangeTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~WaveformRangeTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // Methods to return calibration data
  // Map indexed by digitizer channel number
  // Returns full-scale ADC range as float
  virtual WaveformRangeMap getRangeMapping(const EventContext& ctx) const override;
  virtual WaveformRangeMap getRangeMapping(void) const override;

 private:
  // Read Cond Handle
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/WAVE/DAQ/Range", "Key of range folder"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

};

//---------------------------------------------------------------------- 
#endif // WAVEFORM_CABLE_MAPPING_TOOL
