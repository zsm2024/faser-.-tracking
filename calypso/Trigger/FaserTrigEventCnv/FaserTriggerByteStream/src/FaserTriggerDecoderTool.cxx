/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "FaserTriggerDecoderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "xAODFaserTrigger/FaserTriggerData.h"
#include "xAODFaserTrigger/FaserTriggerDataAux.h"

#include "EventFormats/TLBDataFragment.hpp"


static const InterfaceID IID_IFaserTriggerDecoderTool("FaserTriggerDecoderTool", 1, 0);

const InterfaceID& FaserTriggerDecoderTool::interfaceID() {
  return IID_IFaserTriggerDecoderTool;
}

FaserTriggerDecoderTool::FaserTriggerDecoderTool(const std::string& type, 
      const std::string& name,const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<FaserTriggerDecoderTool>(this);
}

FaserTriggerDecoderTool::~FaserTriggerDecoderTool()
{
}

StatusCode
FaserTriggerDecoderTool::initialize() 
{
  ATH_MSG_DEBUG("FaserTriggerDecoderTool::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode
FaserTriggerDecoderTool::finalize() 
{
  ATH_MSG_DEBUG("FaserTriggerDecoderTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode
FaserTriggerDecoderTool::convert(const DAQFormats::EventFull* re, 
				   xAOD::FaserTriggerData* xaod)
{
  ATH_MSG_DEBUG("FaserTriggerDecoderTool::convert()");

  if (!re) {
    ATH_MSG_ERROR("EventFull passed to convert() is null!");
    return StatusCode::FAILURE;
  }

  // Find the TLB fragment
  const TLBDataFormat::TLBDataFragment* tlb = NULL;
  const DAQFormats::EventFragment* frag = NULL;
  for(const auto &id : re->getFragmentIDs()) {
    frag=re->find_fragment(id);

    if ((frag->source_id()&0xFFFF0000) != DAQFormats::SourceIDs::TriggerSourceID) continue;
    ATH_MSG_DEBUG("Fragment:\n" << *frag);

    tlb = new TLBDataFormat::TLBDataFragment(frag->payload<const uint32_t*>(), frag->payload_size()); 
    break;
  }

  if (!tlb) {
    ATH_MSG_ERROR("Failed to find TLB fragment in raw event!");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("TLB:\n" << *tlb);

  // Finally, set values in xAOD object from TLB
  try {
    xaod->setHeader( tlb->header() );
    xaod->setEventId( tlb->event_id() );
    xaod->setBcid( tlb->bc_id() );
    xaod->setOrbitId( tlb->orbit_id() );
    xaod->setInputBits( tlb->input_bits() );
    xaod->setInputBitsNextClk( tlb->input_bits_next_clk() );
    xaod->setTbp( tlb->tbp() );
    xaod->setTap( tlb->tap() );
  } catch ( TLBDataFormat::TLBDataException& e ) {
    ATH_MSG_WARNING("FaserTriggerDecoderTool:\n"
		    << e.what()
		    << "\nCorrupted TLB data!\n"
		    << *frag);
  }

  // Don't spring a leak
  delete tlb;

  return StatusCode::SUCCESS; 
}
