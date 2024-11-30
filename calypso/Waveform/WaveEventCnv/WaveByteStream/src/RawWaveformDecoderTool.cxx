/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "RawWaveformDecoderTool.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "WaveRawEvent/RawWaveform.h"
#include "EventFormats/DigitizerDataFragment.hpp"

static const InterfaceID IID_IRawWaveformDecoderTool("RawWaveformDecoderTool", 1, 0);

const InterfaceID& RawWaveformDecoderTool::interfaceID() {
  return IID_IRawWaveformDecoderTool;
}

RawWaveformDecoderTool::RawWaveformDecoderTool(const std::string& type, 
      const std::string& name,const IInterface* parent)
  : AthAlgTool(type, name, parent)
{
  declareInterface<RawWaveformDecoderTool>(this);
}

RawWaveformDecoderTool::~RawWaveformDecoderTool()
{
}

StatusCode
RawWaveformDecoderTool::initialize() 
{
  ATH_MSG_DEBUG("RawWaveformDecoderTool::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode
RawWaveformDecoderTool::finalize() 
{
  ATH_MSG_DEBUG("RawWaveformDecoderTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode
RawWaveformDecoderTool::convert(const DAQFormats::EventFull* re, 
				RawWaveformContainer* container,
				const std::string key,
				WaveformCableMap cable_map,
				WaveformRangeMap range_map
)
{
  ATH_MSG_DEBUG("RawWaveformDecoderTool::convert("+key+")");

  if (!re) {
    ATH_MSG_ERROR("EventFull passed to convert() is null!");
    return StatusCode::FAILURE;
  }

  if (!container) {
    ATH_MSG_ERROR("RawWaveformContainer passed to convert() is null!");
    return StatusCode::FAILURE;
  }

  // Find the Waveform fragment
  const DigitizerDataFragment* digitizer = NULL;
  const DigitizerDataFragment* digitizer0 = NULL;
  const DigitizerDataFragment* digitizer1 = NULL;
  const DAQFormats::EventFragment* frag = NULL;
  
  // Dump all fragments for debugging
  // for(const auto &id : re->getFragmentIDs()) {
  //   frag=re->find_fragment(id);
  //   ATH_MSG_DEBUG("Fragment:\n" << *frag);
  // }
  
  for(const auto &id : re->getFragmentIDs()) {
    frag=re->find_fragment(id);

    if ((frag->source_id()&0xFFFF0000) != DAQFormats::SourceIDs::PMTSourceID) continue;
    ATH_MSG_DEBUG("Found Fragment:\n" << *frag);

    if ((frag->source_id()&0x03) == 0)
      digitizer0 = new DigitizerDataFragment(frag->payload<const uint32_t*>(), frag->payload_size());
    else if ((frag->source_id()&0x03) == 1)
      digitizer1 = new DigitizerDataFragment(frag->payload<const uint32_t*>(), frag->payload_size());
    else
      ATH_MSG_WARNING("Couldn't match digitizer with fragment source id!");
  }

  if (!digitizer0) {
    ATH_MSG_WARNING("Failed to find digitizer0 fragment in raw event!");
    return StatusCode::SUCCESS;
  } 
    
  // Check validity here, try to continue, as perhaps not all channels are bad
  if (!digitizer0->valid()) {
    ATH_MSG_WARNING("Found invalid digitizer0 fragment:\n" << *digitizer0);
  } else {
    ATH_MSG_DEBUG("Found valid digitizer0 fragment");
  }


  if (!digitizer1) {
    // Will happen in data before 2024
    ATH_MSG_INFO("Failed to find digitizer1 fragment in raw event!");
  } else {
    if (!digitizer1->valid()) {
      ATH_MSG_WARNING("Found invalid digitizer1 fragment:\n" << *digitizer1);
    } else {
      ATH_MSG_DEBUG("Found valid digitizer1 fragment");
    }
  }    

  // Detector type to match in first element of cable map
  std::string det_type;
  if (key == std::string("CaloWaveforms")) {
    det_type = std::string("calo");
  } else if (key == std::string("Calo2Waveforms")) {
    det_type = std::string("calo2");
  } else if (key == std::string("VetoWaveforms")) {
    det_type = std::string("veto");
  } else if (key == std::string("VetoNuWaveforms")) {
    det_type = std::string("vetonu");
  } else if (key == std::string("TriggerWaveforms")) {
    det_type = std::string("trigger");
  } else if (key == std::string("PreshowerWaveforms")) {
    det_type = std::string("preshower");
  } else if (key == std::string("ClockWaveforms")) {
    det_type = std::string("clock");
  } else {
    ATH_MSG_ERROR("Unknown key " << key);
    return StatusCode::FAILURE;
  }

  // Loop over 32 digitizer channels
  std::vector<int> channelList(32);
  std::iota (std::begin(channelList), std::end(channelList), 0);

  for (int channel: channelList) {

    // Only look at channels we care about
    if (det_type != cable_map[channel].first) continue;

    ATH_MSG_DEBUG("Converting channel "+std::to_string(channel)+" for "+key);

    int chan = channel;
    digitizer = digitizer0;
    if (channel > 15) {
      chan = channel-16;
      digitizer = digitizer1;
    }
    
    // Check if this has data
    if (!digitizer->channel_has_data(chan)) {
      ATH_MSG_DEBUG("Channel " << channel << " has no data - skipping!");
      continue;
    } 

    RawWaveform* wfm = new RawWaveform();

    try {
      wfm->setWaveform( channel, digitizer->channel_adc_counts( chan ) );
    } catch ( DigitizerData::DigitizerDataException& e ) {
      ATH_MSG_WARNING("RawWaveformDecoderTool:\n"
		   <<e.what()
		   << "\nChannel "
		   << channel
		   << " not filled!\n");
    }

    try {
      wfm->setHeader( digitizer );

    } catch ( DigitizerData::DigitizerDataException& e ) {
      ATH_MSG_WARNING("RawWaveformDecoderTool:\n"
		      << e.what()
		      << "\nCorrupted Digitizer data!\n"
		      << *frag);
    }

    // Set ID if one exists (clock, for instance, doesn't have an identifier)
    if (cable_map.at(channel).second != -1) { // Identifier doesn't have operator>=
      wfm->setIdentifier(cable_map[channel].second);
    }

    // Set ADC range
    wfm->setRange(range_map.at(channel));

    container->push_back(wfm);    

    // Sanity check
    if (wfm->adc_counts().size() != wfm->n_samples()) {
      ATH_MSG_WARNING("Created waveform channel" << channel << "with length " << wfm->adc_counts().size() << " but header reports n_samples = " << wfm->n_samples() << "!");
      ATH_MSG_WARNING(*wfm);
    }

  }

  // Don't spring a leak
  delete digitizer0;
  if (digitizer1)
    delete digitizer1;
  
  ATH_MSG_DEBUG( "RawWaveformDecoderTool created container " << key 
		 << " with size=" << container->size());
  return StatusCode::SUCCESS; 
}
