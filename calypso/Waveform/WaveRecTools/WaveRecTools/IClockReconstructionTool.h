/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file IClockReconstructionTool.h
 * Header file for the IClockReconstructionTool class
 * @author Eric Torrence, 2021
 */


#ifndef WAVERECTOOLS_ICLOCKRECONSTRUCTIONTOOL_H
#define WAVERECTOOLS_ICLOCKRECONSTRUCTIONTOOL_H

// Base class
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "xAODFaserWaveform/WaveformClock.h"

class RawWaveform;

///Interface for Clock reco algorithms
class IClockReconstructionTool : virtual public IAlgTool 
{
  public:

  // InterfaceID
  DeclareInterfaceID(IClockReconstructionTool, 1, 0);

  virtual ~IClockReconstructionTool() = default;

  // Reconstruct all peaks in a raw waveform
  virtual StatusCode reconstruct(const RawWaveform& wave, 
				 xAOD::WaveformClock* clockdata) const = 0;

};

#endif // SCINTRECTOOLS_ICLOCKRECONSTRUCTIONTOOL_H
