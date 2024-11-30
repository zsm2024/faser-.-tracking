//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERBYTESTREAM_TRACKERBYTESTREAMCNV_H
#define TRACKERBYTESTREAM_TRACKERBYTESTREAMCNV_H

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthMessaging.h"
#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"
#include "FaserSCT_ConditionsTools/ISCT_CableMappingTool.h"

class TrackerDataDecoderTool;
class IFaserROBDataProviderSvc;
class FaserSCT_ID;
class StoreGateSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class TrackerByteStreamCnv: public Converter, public AthMessaging {

public: 
  TrackerByteStreamCnv(ISvcLocator* svcloc);
  virtual ~TrackerByteStreamCnv();
  
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj) override;  
  /// Storage type and class ID
  virtual long repSvcType() const override { return i_repSvcType(); }
  static long storageType() { return FaserByteStreamAddress::storageType(); }
  static const CLID& classID();
  
private:
  ServiceHandle<IFaserROBDataProviderSvc> m_rdpSvc;
  ServiceHandle<StoreGateSvc>             m_detStoreSvc;
  ToolHandle<TrackerDataDecoderTool>      m_tool;
  ToolHandle<ISCT_CableMappingTool>       m_mappingTool;
  const FaserSCT_ID*                      m_sctID{nullptr};
};

#endif  /* TRACKERBYTESTREAM_TRACKERBYTESTREAMCNV_H */


