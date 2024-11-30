/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef WAVEWAVEFORMCNV_P0_H
#define WAVEWAVEFORMCNV_P0_H

#include "WaveRawEvent/RawWaveform.h"
#include "WaveEventAthenaPool/RawWaveform_p0.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class RawWaveformCnv_p0 : public T_AthenaPoolTPCnvBase<RawWaveform, RawWaveform_p0> {
 public:
  RawWaveformCnv_p0() {};

  virtual void persToTrans(const RawWaveform_p0* persObj,
			   RawWaveform* transObj,
			   MsgStream& log);

  virtual void transToPers(const RawWaveform* transObj,
			   RawWaveform_p0* persObj,
			   MsgStream& log);
 private:
};

#endif
