//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef FASERTRIGCNV_FASERTRIGGERDECODERTOOL_H
#define FASERTRIGCNV_FASERTRIGGERDECODERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "EventFormats/DAQFormats.hpp"

#include "xAODFaserTrigger/FaserTriggerData.h"

// This class provides conversion between bytestream and xAOD trigger objects

class FaserTriggerDecoderTool : public AthAlgTool {

 public:
  FaserTriggerDecoderTool(const std::string& type, const std::string& name, 
			  const IInterface* parent);

  virtual ~FaserTriggerDecoderTool();

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  StatusCode convert(const DAQFormats::EventFull* re, xAOD::FaserTriggerData* xaod);

};

#endif
