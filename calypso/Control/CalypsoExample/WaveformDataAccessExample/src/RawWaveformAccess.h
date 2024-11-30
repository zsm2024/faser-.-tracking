/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

/*
 * RawWaveformAccess.h
 * 
 * Simple algorithm to access digitizer data from storegate.
 * Try to write a proper thread-safe algorithm.
 *
 */

#ifndef WAVEFORMDATAACCESSEXAMPLE_RAWWAVEFORMACCESS_H
#define WAVEFORMDATAACCESSEXAMPLE_RAWWAVEFORMACCESS_H 

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"

#include "WaveRawEvent/RawWaveformContainer.h"

class RawWaveformAccess: public AthReentrantAlgorithm
{
 public:
  RawWaveformAccess(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~RawWaveformAccess();

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:
  /// StoreGate key
  SG::ReadHandleKey<RawWaveformContainer> m_CaloWaveformContainer
    { this, "CaloWaveformContainerKey", "CaloWaveforms", "ReadHandleKey for CaloWaveforms Container"};
  SG::ReadHandleKey<RawWaveformContainer> m_Calo2WaveformContainer
    { this, "Calo2WaveformContainerKey", "Calo2Waveforms", "ReadHandleKey for Calo2Waveforms Container"};
  SG::ReadHandleKey<RawWaveformContainer> m_VetoWaveformContainer
    { this, "VetoWaveformContainerKey", "VetoWaveforms", "ReadHandleKey for CaloWaveforms Container"};
  SG::ReadHandleKey<RawWaveformContainer> m_TriggerWaveformContainer
    { this, "TriggerWaveformContainerKey", "TriggerWaveforms", "ReadHandleKey for TriggerWaveforms Container"};
  SG::ReadHandleKey<RawWaveformContainer> m_PreshowerWaveformContainer
    { this, "PreshowerWaveformContainerKey", "PreshowerWaveforms", "ReadHandleKey for PreshowerWaveforms Container"};
  SG::ReadHandleKey<RawWaveformContainer> m_ClockWaveformContainer
    { this, "ClockWaveformContainerKey", "ClockWaveforms", "ReadHandleKey for ClockWaveforms Container"};
};

#endif /* WAVEFORMDATAACCESSEXAMPLE_RAWWAVEFORMACCESS_H */
