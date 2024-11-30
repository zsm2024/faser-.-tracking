/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "FaserTriggerByteStreamCnv.h"

#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"
#include "xAODFaserTrigger/FaserTriggerData.h"
#include "xAODFaserTrigger/FaserTriggerDataAux.h"
#include "EventFormats/DAQFormats.hpp"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"

FaserTriggerByteStreamCnv::FaserTriggerByteStreamCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , AthMessaging(svcloc != nullptr ? msgSvc() : nullptr, "FaserTriggerByteStreamCnv")
{
}

FaserTriggerByteStreamCnv::~FaserTriggerByteStreamCnv() {
}

const CLID& FaserTriggerByteStreamCnv::classID() 
{
  // Change to our data object
  return ClassID_traits<xAOD::FaserTriggerData>::ID();
}

StatusCode FaserTriggerByteStreamCnv::initialize() 
{
  ATH_MSG_DEBUG("initialize");

  CHECK(Converter::initialize());
  return StatusCode::SUCCESS;
}

StatusCode FaserTriggerByteStreamCnv::finalize() 
{
  ATH_MSG_DEBUG("finalize");

  CHECK(Converter::finalize());
  return StatusCode::SUCCESS;  
}

StatusCode FaserTriggerByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  FaserByteStreamAddress *pRE_Addr{nullptr};
  pRE_Addr = dynamic_cast<FaserByteStreamAddress*>(pAddr);
  if (!pRE_Addr) {
    ATH_MSG_ERROR("Cannot cast to FaserByteStreamAddress ");
    return StatusCode::FAILURE;
  }
               
  ATH_MSG_DEBUG("Creating Objects");

  const std::string nm = *(pRE_Addr->par());
  const std::string nmAux = nm + "Aux.";

  xAOD::FaserTriggerData* pTriggerData = new xAOD::FaserTriggerData();
  DataLink<xAOD::FaserTriggerDataAux> link(nmAux);
  pTriggerData->setStore(link);
  pObj = SG::asStorable(pTriggerData);

  ATH_MSG_DEBUG("New FaserTriggerData made");

  return StatusCode::SUCCESS;

}


