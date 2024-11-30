/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "RawWaveformContainerCnv_p0.h"
#include "RawWaveformCnv_p0.h"

void
RawWaveformContainerCnv_p0::persToTrans(const RawWaveformContainer_p0* persCont, RawWaveformContainer* transCont, MsgStream& log) {

  log << MSG::DEBUG << "RawWaveformContainerCnv_p0::persToTrans called" << endmsg;

  RawWaveformCnv_p0 waveformCnv;

  // Clear this, transient container is DataVector, which stores pointers
  // Create them below and push back to fill
  transCont->clear();

  for (unsigned int index = 0; index < persCont->m_waveforms.size(); ++index) {
    std::unique_ptr<RawWaveform> data = std::make_unique<RawWaveform>();
    const RawWaveform_p0* pdata = &persCont->m_waveforms.at(index);
    waveformCnv.persToTrans(pdata, data.get(), log);
    
    // Fill other values held by container in persistent class
    data->setBoardFailFlag(persCont->m_board_fail_flag);
    data->setBoardId(persCont->m_board_id);
    data->setPatternTrigOptions(persCont->m_pattern_trig_options);
    data->setChannelMask(persCont->m_channel_mask);
    data->setEventCounter(persCont->m_event_counter);
    data->setTriggerTime(persCont->m_trigger_time_tag);
    data->setSamples(persCont->m_samples);

    transCont->push_back(data.release());
  }
}

void
RawWaveformContainerCnv_p0::transToPers(const RawWaveformContainer* transCont,RawWaveformContainer_p0* persCont, MsgStream& log) {
  // log << MSG::DEBUG << "transCont = " << transCont << " / persCont = " << persCont << endmsg;
  log << MSG::DEBUG << "RawWaveformContainerCnv_p0::transToPers preparing " << transCont->size() << " waveforms" << endmsg;

  // If trans container is empty, nothing else to do
  if (!transCont->size()) {
    log << MSG::DEBUG << "RawWaveformContainerCnv_p0::transToPers found empty container, exiting!" << endmsg;
    return;
  }

  RawWaveformCnv_p0 waveformCnv;

  typedef RawWaveformContainer TRANS;
  TRANS::const_iterator it_Cont = transCont->begin();
  TRANS::const_iterator it_ContEnd = transCont->end();

  // Fill common information
  const RawWaveform* transObj = (*it_Cont);
  persCont->m_board_fail_flag = transObj->board_fail_flag();
  persCont->m_board_id = transObj->board_id();
  persCont->m_pattern_trig_options = transObj->pattern_trig_options();
  persCont->m_channel_mask = transObj->channel_mask();
  persCont->m_event_counter = transObj->event_counter();
  persCont->m_trigger_time_tag = transObj->trigger_time_tag();
  persCont->m_samples = transObj->n_samples();

  // Loop over each waveform and convert those as well
  persCont->m_waveforms.resize(transCont->size());

  for (int index=0; it_Cont != it_ContEnd; it_Cont++, index++) {
    // Add new collection
    log << MSG::DEBUG << "Converting waveform " << index << endmsg;
    const RawWaveform* data = (*it_Cont);
    RawWaveform_p0* pdata = &(persCont->m_waveforms.at(index));
    waveformCnv.transToPers(data, pdata, log);
  }

  log << MSG::DEBUG << "RawWaveformContainerCnv_p0::transToPers finished" << endmsg;

}
