/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "WaveByteStreamCnv.h"
#include "RawWaveformDecoderTool.h"

#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"
#include "WaveRawEvent/RawWaveform.h"
#include "WaveRawEvent/RawWaveformContainer.h"
#include "EventFormats/DAQFormats.hpp"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IToolSvc.h"

using DAQFormats::EventFull;

WaveByteStreamCnv::WaveByteStreamCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , AthMessaging(svcloc != nullptr ? msgSvc() : nullptr, "WaveByteStreamCnv")
  , m_name("WaveByteStreamCnv")
  , m_tool("RawWaveformDecoderTool")
  , m_mappingTool("WaveformCableMappingTool")
  , m_rangeTool("WaveformRangeTool")
  , m_rdpSvc("FaserROBDataProviderSvc", m_name)
{
  ATH_MSG_DEBUG(m_name+"::initialize() called");
}

WaveByteStreamCnv::~WaveByteStreamCnv() {
}

const CLID& WaveByteStreamCnv::classID() 
{
  // Change to our data object
  return ClassID_traits<RawWaveformContainer>::ID();
}

StatusCode WaveByteStreamCnv::initialize() 
{
  ATH_MSG_DEBUG(m_name+"::initialize() called");

  CHECK(Converter::initialize());
  CHECK(m_rdpSvc.retrieve());
  CHECK(m_tool.retrieve());
  CHECK(m_mappingTool.retrieve());
  CHECK(m_rangeTool.retrieve());
  return StatusCode::SUCCESS;
}

StatusCode WaveByteStreamCnv::finalize() 
{
  ATH_MSG_DEBUG("WaveByteStreamCnv::Finalize");

  CHECK(Converter::finalize());
  return StatusCode::SUCCESS;  

}

StatusCode WaveByteStreamCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  ATH_MSG_DEBUG("WaveByteStreamCnv::createObj() called");

  // Check that we can access raw data
  if (!m_rdpSvc) {
    ATH_MSG_ERROR("WaveByteStreamCnv::createObj() - ROBDataProviderSvc not loaded!");
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

  // Get key used in the StoreGateSvc::retrieve function
  const std::string key = *(pRE_Addr->par());
  ATH_MSG_DEBUG("WaveByteStreamCnv - creating objects "+key);

  RawWaveformContainer* wfmCont = new RawWaveformContainer;

  // Get mapping tool
  auto mapping = m_mappingTool->getCableMapping();
  ATH_MSG_DEBUG("Cable mapping contains " << mapping.size() << " entries");

  auto range = m_rangeTool->getRangeMapping();
  ATH_MSG_DEBUG("Range contains " << range.size() << " entries");

  // Convert selected channels
  CHECK( m_tool->convert(re, wfmCont, key, mapping, range) );
  
  pObj = SG::asStorable(wfmCont);

  ATH_MSG_DEBUG(" New xAOD::RawWaveformData created");
  return StatusCode::SUCCESS;
}

