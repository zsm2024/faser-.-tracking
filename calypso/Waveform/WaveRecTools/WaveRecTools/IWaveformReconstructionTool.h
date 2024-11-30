/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file IWaveformReconstructionTool.h
 * Header file for the IWaveformReconstructionTool class
 * @author Eric Torrence, 2021
 */


#ifndef WAVERECTOOLS_IWAVEFORMRECONSTRUCTIONTOOL_H
#define WAVERECTOOLS_IWAVEFORMRECONSTRUCTIONTOOL_H

// Base class
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformClock.h"

class RawWaveform;
class RawWaveformContainer;

///Interface for Waveform reco algorithms
class IWaveformReconstructionTool : virtual public IAlgTool 
{
  public:

  // InterfaceID
  DeclareInterfaceID(IWaveformReconstructionTool, 1, 0);

  virtual ~IWaveformReconstructionTool() = default;

  // Reconstruct hits in trigger window
  virtual StatusCode reconstructPrimary(const RawWaveform& wave, 
					xAOD::WaveformHitContainer* container) const = 0;

  // Reconstruct secondary hits anywhere in the waveform
  virtual StatusCode reconstructSecondary(const RawWaveform& wave, 
					  xAOD::WaveformHitContainer* container) const = 0;

  // Set local hit times from LHC clock
  virtual StatusCode setLocalTime(const xAOD::WaveformClock* clock,
				  xAOD::WaveformHitContainer* container) const = 0;

};

#endif // SCINTRECTOOLS_IWAVEFORMRECONSTRUCTIONTOOL_H
