/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

/*
 * RawWaveformAccess.cxx
 * 
 * Simple algorithm to access waveform data from storegate.
 * Try to write a proper thread-safe algorithm.
 *
 */

#include "RawWaveformAccess.h"

RawWaveformAccess::RawWaveformAccess(const std::string& name, ISvcLocator* pSvcLocator) : AthReentrantAlgorithm(name, pSvcLocator)
{
}

RawWaveformAccess::~RawWaveformAccess()
{
}

StatusCode
RawWaveformAccess::initialize()
{
  ATH_MSG_DEBUG("RawWaveformAccess::initialize() called");

  // Must initialize SG handles
  ATH_CHECK( m_CaloWaveformContainer.initialize() );
  ATH_CHECK( m_Calo2WaveformContainer.initialize() );
  ATH_CHECK( m_VetoWaveformContainer.initialize() );
  ATH_CHECK( m_TriggerWaveformContainer.initialize() );
  ATH_CHECK( m_PreshowerWaveformContainer.initialize() );
  ATH_CHECK( m_ClockWaveformContainer.initialize() );

  return StatusCode::SUCCESS;
}

StatusCode
RawWaveformAccess::finalize()
{
  ATH_MSG_DEBUG("RawWaveformAccess::finalize() called");

  return StatusCode::SUCCESS;
}

StatusCode
RawWaveformAccess::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("RawWaveformAccess::execute() called");

  // Try reading all of the different containers
  SG::ReadHandle<RawWaveformContainer> caloHandle(m_CaloWaveformContainer, ctx);
  ATH_MSG_INFO("Found ReadHandle for CaloWaveforms");
  ATH_MSG_INFO(*caloHandle);

  SG::ReadHandle<RawWaveformContainer> calo2Handle(m_Calo2WaveformContainer, ctx);
  ATH_MSG_INFO("Found ReadHandle for Calo2Waveforms");
  ATH_MSG_INFO(*caloHandle);

  SG::ReadHandle<RawWaveformContainer> vetoHandle(m_VetoWaveformContainer, ctx);
  ATH_MSG_INFO("Found ReadHandle for VetoWaveforms");
  ATH_MSG_INFO(*vetoHandle);

  SG::ReadHandle<RawWaveformContainer> triggerHandle(m_TriggerWaveformContainer, ctx);
  ATH_MSG_INFO("Found ReadHandle for TriggerWaveforms");
  ATH_MSG_INFO(*triggerHandle);

  SG::ReadHandle<RawWaveformContainer> preshowerHandle(m_PreshowerWaveformContainer, ctx);
  ATH_MSG_INFO("Found ReadHandle for PreshowerWaveforms");
  ATH_MSG_INFO(*preshowerHandle);

  SG::ReadHandle<RawWaveformContainer> clockHandle(m_ClockWaveformContainer, ctx);
  ATH_MSG_INFO("Found ReadHandle for ClockWaveforms");
  ATH_MSG_INFO(*clockHandle);

  return StatusCode::SUCCESS;
}
