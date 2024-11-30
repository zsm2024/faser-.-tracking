//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef WAVEBYTESTREAM_WAVEBYTESTREAMCNV_H
#define WAVEBYTESTREAM_WAVEBYTESTREAMCNV_H

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"
#include "WaveformConditionsTools/IWaveformCableMappingTool.h"
#include "WaveformConditionsTools/IWaveformRangeTool.h"

class RawWaveformDecoderTool;
class IFaserROBDataProviderSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class WaveByteStreamCnv: public Converter, public AthMessaging {

public: 
  WaveByteStreamCnv(ISvcLocator* svcloc);
  virtual ~WaveByteStreamCnv();
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;  
  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType() { return FaserByteStreamAddress::storageType(); }
  static const CLID& classID();
  
private:
  std::string m_name;
  ToolHandle<RawWaveformDecoderTool> m_tool;
  ToolHandle<IWaveformCableMappingTool> m_mappingTool;
  ToolHandle<IWaveformRangeTool> m_rangeTool;
  ServiceHandle<IFaserROBDataProviderSvc> m_rdpSvc;
};

#endif  /* WAVEBYTESTREAM_WAVEBYTESTREAMCNV_H */


