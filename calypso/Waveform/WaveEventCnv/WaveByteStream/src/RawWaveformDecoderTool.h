//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef WAVEBYTESTREAM_RAWWAVEFORMDECODERTOOL_H
#define WAVEBYTESTREAM_RAWWAVEFORMDECODERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "EventFormats/DAQFormats.hpp"
#include "WaveRawEvent/RawWaveformContainer.h"

#include "WaveformConditionsTools/IWaveformCableMappingTool.h"
#include "WaveformConditionsTools/IWaveformRangeTool.h"

// This class provides conversion between bytestream and Waveform objects

class RawWaveformDecoderTool : public AthAlgTool {

 public:
  RawWaveformDecoderTool(const std::string& type, const std::string& name, 
			  const IInterface* parent);

  virtual ~RawWaveformDecoderTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convert(const DAQFormats::EventFull* re, RawWaveformContainer* wfm, std::string key, WaveformCableMap cable_map, WaveformRangeMap range_map);

private:
};

#endif  /* WAVEBYTESTREAM_FASERTRIGGERDECODERTOOL_H */
 
