/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserTriggerByteStreamAuxCnv.h"
#include "FaserTriggerDecoderTool.h"

#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"

#include "EventFormats/DAQFormats.hpp"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "xAODFaserTrigger/FaserTriggerData.h"
#include "xAODFaserTrigger/FaserTriggerDataAux.h"

#include "StoreGate/StoreGateSvc.h"

#include "eformat/StreamTag.h"

#include <time.h>

using DAQFormats::EventFull;

FaserTriggerByteStreamAuxCnv::FaserTriggerByteStreamAuxCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , AthMessaging(svcloc != nullptr ? msgSvc() : nullptr, "FaserTriggerByteStreamAuxCnv")
  , m_tool("FaserTriggerDecoderTool")
  , m_robDataProvider("FaserROBDataProviderSvc", "FaserTriggerByteStreamAuxCnv")
{
}

const CLID& FaserTriggerByteStreamAuxCnv::classID() 
{
  return ClassID_traits<xAOD::FaserTriggerDataAux>::ID();
}

long FaserTriggerByteStreamAuxCnv::storageType() 
{
  return FaserByteStreamAddress::storageType();
}

StatusCode FaserTriggerByteStreamAuxCnv::initialize() 
{
  ATH_MSG_DEBUG("Initialize");

  CHECK(Converter::initialize());

  CHECK(m_robDataProvider.retrieve());
  CHECK(m_tool.retrieve());

  return StatusCode::SUCCESS;
}

StatusCode FaserTriggerByteStreamAuxCnv::finalize() 
{
  ATH_MSG_DEBUG("Finalize");
  
  StatusCode sc = Converter::finalize();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Converter::finalize() failed");
  }
  return sc;
}

StatusCode FaserTriggerByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  if (pAddr == nullptr) ATH_MSG_ERROR("pAddr is null");
  FaserByteStreamAddress *pRE_Addr{nullptr};
  pRE_Addr = dynamic_cast<FaserByteStreamAddress*>(pAddr);
  if (!pRE_Addr) {
    ATH_MSG_ERROR("Cannot cast to FaserByteStreamAddress ");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Creating Objects");

  // get RawEvent
  const EventFull* re = m_robDataProvider->getEvent();
  if (!re) {
    ATH_MSG_ERROR("Can not get RawEvent ");
    return StatusCode::FAILURE;
  }

  // Run Number
  int runNumber = re->run_number();

  // Event Number
  uint64_t eventNumber;
  eventNumber = re->event_id();


  xAOD::FaserTriggerData triggerData;
  xAOD::FaserTriggerDataAux* pTriggerDataAux = new xAOD::FaserTriggerDataAux();
  triggerData.setStore(pTriggerDataAux);

  CHECK( m_tool->convert(re, &triggerData) );

  pObj = SG::asStorable(pTriggerDataAux);

  ATH_MSG_DEBUG("Created xAOD::FaserTriggerDataAux for run/event =" << runNumber << " / " << eventNumber);
  return StatusCode::SUCCESS;
}

StatusCode FaserTriggerByteStreamAuxCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) 
{
  ATH_MSG_DEBUG("Nothing to be done for xAOD::FaserTriggerDataAux createReps");
  return StatusCode::SUCCESS;
}

