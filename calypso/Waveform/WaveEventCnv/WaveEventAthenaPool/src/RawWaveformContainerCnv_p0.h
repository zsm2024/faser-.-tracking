/*
 Copyright 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef WAVEWAVEFORMCONTAINERCNV_P0_H
#define WAVEWAVEFORMCONTAINERCNV_P0_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "WaveRawEvent/RawWaveformContainer.h"
#include "WaveEventAthenaPool/RawWaveformContainer_p0.h"

class RawWaveformContainerCnv_p0 : public T_AthenaPoolTPCnvBase<RawWaveformContainer, RawWaveformContainer_p0> {

 public:
  RawWaveformContainerCnv_p0() {};

  virtual void persToTrans(const RawWaveformContainer_p0* persCont,
			   RawWaveformContainer* transCont, 
			   MsgStream& log);

  virtual void transToPers(const RawWaveformContainer* transCont, 
			   RawWaveformContainer_p0* persCont, 
			   MsgStream& log);

};

#endif
