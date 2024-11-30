/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file IWaveformDigitisationTool.h
 * Header file for the IWaveformDigitisationTool class
 * @author Carl Gwilliam, 2021
 */


#ifndef WAVEDIGITOOLS_IWAVEFORMDIGITISATIONTOOL_H
#define WAVEDIGITOOLS_IWAVEFORMDIGITISATIONTOOL_H

// Base class
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "WaveRawEvent/RawWaveformContainer.h"
#include "WaveRawEvent/RawWaveform.h"
#include "ScintSimEvent/ScintHitCollection.h"
#include "FaserCaloSimEvent/CaloHitCollection.h"

#include "Identifier/Identifier.h"

#include "TF1.h"
#include "TRandom3.h"
#include "TH1D.h"

#include <utility>
#include <map>
#include <vector>

// Forward declaration
class IWaveformDigiConditionsTool;

typedef std::map<Identifier, TH1D*> EvstHistMap;

///Interface for waveform digitisation tools
class IWaveformDigitisationTool : virtual public IAlgTool 
{
public:

  // InterfaceID
  DeclareInterfaceID(IWaveformDigitisationTool, 1, 0);

  IWaveformDigitisationTool():
    m_msgSvc         ( "MessageSvc",   "ITrkEventCnvTool" )
  {}

  virtual ~IWaveformDigitisationTool() = default;

  /// Create waveforms - old method
  virtual std::map<Identifier, std::vector<uint16_t>>& generate_scint_waveforms(
		const EventContext& ctx,
	      	const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const ScintHitCollection* hitCollection) const = 0;

  /// Create waveforms using hit timing
  virtual std::map<Identifier, std::vector<uint16_t>>& generate_scint_timing_waveforms(
		const EventContext& ctx,
	      	const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const ScintHitCollection* hitCollection) const = 0;

  /// Create waveforms - old method
  virtual std::map<Identifier, std::vector<uint16_t>>& generate_calo_waveforms(
		const EventContext& ctx,
	      	const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const CaloHitCollection* hitCollection) const = 0;

  /// Create waveforms using hit timing
  virtual std::map<Identifier, std::vector<uint16_t>>& generate_calo_timing_waveforms(
		const EventContext& ctx,
	      	const ToolHandle<IWaveformDigiConditionsTool>& digiCondTool,
		const CaloHitCollection* hitCollection) const = 0;


  /* /// Evaluate time kernel over time samples   */
  /* virtual std::vector<float> evaluate_timekernel(TF1* kernel) const = 0; */

  /* /// Generate random baseline  */
  /* virtual float generate_baseline(float mean, float rms) const = 0; */
 
  /* /// Create structure to store pulse for each channel */
  template <class T> 
  std::map<Identifier, std::vector<uint16_t>> create_waveform_map(const T* idHelper) const; 

  /// Number of time samples
  virtual unsigned int digitizer_samples() const = 0;
  virtual float digitizer_period() const = 0;  // in ns
  virtual unsigned int over_samples() const = 0;
  virtual unsigned int pre_samples() const = 0;

  /// Fill histograms
  //virtual void fill_evst_hist(EvstHistMap& map, const Identifier& id, float time, float energy) const = 0;

  // Remove old histograms
  //virtual void clear_evst_hist(EvstHistMap& map) const = 0;

private:
  ServiceHandle<IMessageSvc>      m_msgSvc;

protected:
  TRandom3*                       m_random;

};

#include "WaveDigiTools/IWaveformDigitisationTool.icc"


#endif //WAVEDIGITOOLS_IWAVEFORMDIGITISATIONTOOL_H
