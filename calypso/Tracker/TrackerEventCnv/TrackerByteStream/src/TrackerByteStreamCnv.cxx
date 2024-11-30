/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "TrackerByteStreamCnv.h"
#include "TrackerDataDecoderTool.h"

#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "EventFormats/DAQFormats.hpp"

#include "TrackerIdentifier/FaserSCT_ID.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"

using DAQFormats::EventFull;

TrackerByteStreamCnv::TrackerByteStreamCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , AthMessaging(svcloc != nullptr ? msgSvc() : nullptr, "TrackerByteStreamCnv")
  , m_tool("TrackerDataDecoderTool")
  , m_mappingTool("FaserSCT_CableMappingTool")
  , m_rdpSvc("FaserROBDataProviderSvc", "TrackerByteStreamCnv")
  , m_detStoreSvc("StoreGateSvc/DetectorStore", "TrackerByteStreamCnv")
{
}

TrackerByteStreamCnv::~TrackerByteStreamCnv() {
}

const CLID& TrackerByteStreamCnv::classID() 
{
  return ClassID_traits<FaserSCT_RDO_Container>::ID();
}

StatusCode TrackerByteStreamCnv::initialize() 
{
  ATH_MSG_DEBUG("initialize() called");

  CHECK(Converter::initialize());
  CHECK(m_rdpSvc.retrieve());
  CHECK(m_tool.retrieve());
  CHECK(m_mappingTool.retrieve());

  ATH_CHECK(m_detStoreSvc.retrieve());
  ATH_CHECK(m_detStoreSvc->retrieve(m_sctID, "FaserSCT_ID"));

  return StatusCode::SUCCESS;
}

StatusCode TrackerByteStreamCnv::finalize() 
{
  ATH_MSG_DEBUG("finalize() called");

  CHECK(Converter::finalize());
  return StatusCode::SUCCESS;  

}

StatusCode TrackerByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  ATH_MSG_DEBUG("createObj() called");

  // Check that we can access raw data
  if (!m_rdpSvc) {
    ATH_MSG_ERROR("createObj() - ROBDataProviderSvc not loaded!");
    return StatusCode::FAILURE;
  }

  FaserByteStreamAddress *pRE_Addr{nullptr};
  pRE_Addr = dynamic_cast<FaserByteStreamAddress*>(pAddr); // Cast from OpaqueAddress
  if (!pRE_Addr) {
    ATH_MSG_ERROR("Cannot cast to FaserByteStreamAddress ");
    return StatusCode::FAILURE;
  }

  // Get pointer to the raw event
  const EventFull* re = m_rdpSvc->getEvent();
  if (!re) {
    ATH_MSG_ERROR("Cannot get raw event from FaserByteStreamInputSvc!");
    return StatusCode::FAILURE;
  }               

  // Get key used in the StoreGateSvc::retrieve function if useful
  const std::string key = *(pRE_Addr->par());
  ATH_MSG_DEBUG("Creating objects "+key);

  unsigned int max = m_sctID->wafer_hash_max(); 
  FaserSCT_RDO_Container* cont = new FaserSCT_RDO_Container(max);
  ATH_MSG_DEBUG("New FaserSCT_RDO_Container created of size="<<cont->size());

  auto mapping = m_mappingTool->getCableMapping();
  ATH_MSG_DEBUG("Cable mapping contains " << mapping.size() << " entries");

  // Convert raw data into this container

  CHECK( m_tool->convert(re, cont, key, mapping) );
  
  pObj = SG::asStorable(cont);

  return StatusCode::SUCCESS;
}

