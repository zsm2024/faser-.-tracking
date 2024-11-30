/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file PseudoSimToWaveformRec.cxx
 * Implementation file for the PseudoSimToWaveformRec class
 * @ author E. Torrence, 2021
 **/

#include "PseudoSimToWaveformRecTool.h"

#include "Identifier/Identifier.h"
#include "xAODFaserWaveform/WaveformHit.h"

// Constructor
PseudoSimToWaveformRecTool::PseudoSimToWaveformRecTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

// Initialization
StatusCode
PseudoSimToWaveformRecTool::initialize() {
  ATH_MSG_INFO( name() << "::initalize()" );

  ATH_CHECK( m_mappingTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode 
PseudoSimToWaveformRecTool::make_hits(const std::map<Identifier, float>& sums,
					xAOD::WaveformHitContainer* container) const {

// Check the container
  if (!container) {
    ATH_MSG_ERROR("HitCollection passed to reconstruct() is null!");
    return StatusCode::FAILURE;
  }

  // Get the nominal trigger time (in ns) from config
  // For now, hack up the timing to match the configuration
  //float trigger_time = m_timingTool->nominalTriggerTime();
  //float offset;

  for (const auto& id : sums) {
    xAOD::WaveformHit* hit = new xAOD::WaveformHit();
    container->push_back(hit);

    hit->set_identifier(id.first);
    hit->set_channel(m_mappingTool->getChannelMapping(id.first));
    hit->set_peak(0);
    hit->set_mean(0);
    hit->set_width(0);
    hit->set_integral(id.second); 
    hit->set_localtime(0);
    hit->set_trigger_time(0);
    hit->set_raw_peak(0);
    hit->set_raw_integral(0); 
  }
  
  return StatusCode::SUCCESS;
}

