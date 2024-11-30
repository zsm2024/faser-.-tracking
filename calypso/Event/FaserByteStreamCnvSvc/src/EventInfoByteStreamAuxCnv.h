/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASEREVENTINFOBYTESTREAMAUXCNV_H
#define FASEREVENTINFOBYTESTREAMAUXCNV_H

/**
 * @file EventInfoByteStreamAuxCnv.h
 *
 * @class EventInfoByteStreamAuxCnv
 *
 * @brief This is the class definition of ByteStream converter for xAOD::EventInfoAux
 * Event Info is built from RawEvent when reading. Nothing is done when writing ByteStream
 *
 */ 

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMessaging.h"

class FaserByteStreamCnvSvc;
class IFaserROBDataProviderSvc;
class StoreGateSvc;

// Abstract factory to create the converter
template <class TYPE> class CnvFactory;

class EventInfoByteStreamAuxCnv : public Converter, public AthMessaging
{
 public:
  EventInfoByteStreamAuxCnv(ISvcLocator* svcloc);
  virtual ~EventInfoByteStreamAuxCnv() override {}
  
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
  const char* ascTime(unsigned int t);    //!< convert timestamp to ascii time.
  FaserByteStreamCnvSvc* m_ByteStreamCnvSvc;   //!< pointer to BS CnvSvc
  ServiceHandle<IFaserROBDataProviderSvc> m_robDataProvider; //!< RODDataProviderSvc handle
  //ServiceHandle<StoreGateSvc> m_mdSvc;                  //!< TDS handle
  
  // flags for EventType
  bool m_isSimulation;
  bool m_isTestbeam;
  bool m_isCalibration;
};

#endif
