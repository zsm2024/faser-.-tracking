// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and CERN collaborations
*/

/** @file WaveformDigiConditionsTool.h Header file for WaveformDigiConditionsTool.
    @author Eric Torrence, 20/04/22
*/

// Multiple inclusion protection
#ifndef WAVEFORM_DIGI_CONDITIONS_TOOL
#define WAVEFORM_DIGI_CONDITIONS_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "WaveformConditionsTools/IWaveformDigiConditionsTool.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

/** This class contains a Tool that reads Waveform digi data and makes it available 
    to other algorithms. 
*/

class WaveformDigiConditionsTool: public extends<AthAlgTool, IWaveformDigiConditionsTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  WaveformDigiConditionsTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~WaveformDigiConditionsTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // Methods to return digigitization data

  // Baseline parameters
  virtual float base_mean(void) const override;
  virtual float base_mean(const EventContext& ctx) const override;

  virtual float base_rms(void) const override;
  virtual float base_rms(const EventContext& ctx) const override;

  // Crystal ball parameters
  virtual float cb_mean(void) const override;
  virtual float cb_mean(const EventContext& ctx) const override;

  virtual float cb_sigma(void) const override;
  virtual float cb_sigma(const EventContext& ctx) const override;

  virtual float cb_norm(void) const override;
  virtual float cb_norm(const EventContext& ctx) const override;

  virtual float cb_alpha(void) const override;
  virtual float cb_alpha(const EventContext& ctx) const override;

  virtual float cb_n(void) const override;
  virtual float cb_n(const EventContext& ctx) const override;

  // Time dependence parameters (probably should do this differently)
  virtual float preshower_time_slope(void) const override;
  virtual float preshower_time_slope(const EventContext& ctx) const override;

  virtual float trigger_time_slope(void) const override;
  virtual float trigger_time_slope(const EventContext& ctx) const override;

 private:

  // COOL channel specifier (Calo, VetoNu, Veto, Trigger or Timing, Preshower)
  Gaudi::Property<std::string> m_detector{this, "Detector", "", "Detector type"};

  // Read Cond Handle
  SG::ReadCondHandleKey<CondAttrListCollection> m_digiReadKey{this, "DigiReadKey", "/WAVE/Digitization", "Key of digi conditions folder"};

  // COOL channel specifier in channel id
  int m_cool_channel;

  float get_value(const EventContext& ctx, std::string argument) const;

  enum COOL_Channel {
    COOL_Calo,
    COOL_VetoNu,
    COOL_Veto,
    COOL_Timing,
    COOL_Preshower
  };
  
};

//---------------------------------------------------------------------- 
#endif // WAVEFORM_DIGI_CONDITIONS_TOOL
