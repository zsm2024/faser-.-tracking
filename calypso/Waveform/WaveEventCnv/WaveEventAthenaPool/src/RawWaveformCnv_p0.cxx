/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#include "RawWaveformCnv_p0.h"

void
RawWaveformCnv_p0::persToTrans(const RawWaveform_p0* persObj, RawWaveform* transObj, MsgStream& /*log*/) {

  // Just fill available data here
  // Rest of it patched up in RawWaveformContainerCnv_p0
  // Persistent object stores ID value, so instantiate Identifier here
  transObj->setIdentifier(Identifier32(persObj->m_ID));
  transObj->setChannel(persObj->m_channel);
  transObj->setCounts(persObj->m_adc_counts);

}

void
RawWaveformCnv_p0::transToPers(const RawWaveform* transObj, RawWaveform_p0* persObj, MsgStream& /*log*/) {

  // log << MSG::DEBUG << "RawWaveformCnv_p0::transToPers called" << endmsg;
  // log << MSG::DEBUG << "Transient waveform:" << endmsg;
  // log << MSG::DEBUG << (*transObj) << endmsg;
  // log << MSG::DEBUG << "Persistent waveform (before):" << endmsg;
  // persObj->print();

  // Save actual ID number 
  persObj->m_ID = transObj->identify32().get_compact();
  persObj->m_channel = transObj->channel();

  // Use copy here
  persObj->m_adc_counts = transObj->adc_counts();

  // log << MSG::DEBUG << "Persistent waveform (after):" << endmsg;
  // persObj->print();
  // log << MSG::DEBUG << "RawWaveformCnv_p0::transToPers done" << endmsg;

}
