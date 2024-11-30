//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERTRIGCNV_FASERTRIGGERBYTESTREAMCNV_H
#define FASERTRIGCNV_FASERTRIGGERBYTESTREAMCNV_H

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class FaserTriggerByteStreamCnv: public Converter, public AthMessaging {

public: 
  FaserTriggerByteStreamCnv(ISvcLocator* svcloc);

  virtual ~FaserTriggerByteStreamCnv();
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;  
  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType() { return FaserByteStreamAddress::storageType(); }
  static const CLID& classID();
  
};

#endif  /* FASERTRIGCNV_FASERTRIGGERBYTESTREAMCNV_H */


