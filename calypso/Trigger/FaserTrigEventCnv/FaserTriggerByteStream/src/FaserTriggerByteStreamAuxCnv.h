/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERTRIGGERBYTESTREAMAUXCNV_H
#define FASERTRIGGERBYTESTREAMAUXCNV_H

/**
 * @file FaserTriggerByteStreamAuxCnv.h
 *
 * @class FaserTriggerByteStreamAuxCnv
 *
 * @brief This is the class definition of ByteStream converter for xAOD::FaserTriggerDataAux
 * Trigger Data is built from RawEvent when reading. Nothing is done when writing ByteStream
 *
 */ 
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMessaging.h"
#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"

class FaserTriggerDecoderTool;
// class IFaserROBDataProviderSvc;
class StoreGateSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class FaserTriggerByteStreamAuxCnv : public Converter, public AthMessaging
{
 public:
  FaserTriggerByteStreamAuxCnv(ISvcLocator* svcloc);
  virtual ~FaserTriggerByteStreamAuxCnv() override {}
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  /// converter method to create object
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;
  /// converter method to write object
  virtual StatusCode createRep(DataObject* pObj, IOpaqueAddress*& pAddr) override;

  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType();
  static const CLID& classID();

 private:
  ToolHandle<FaserTriggerDecoderTool> m_tool;
  ServiceHandle<IFaserROBDataProviderSvc> m_robDataProvider;
  
};

#endif
