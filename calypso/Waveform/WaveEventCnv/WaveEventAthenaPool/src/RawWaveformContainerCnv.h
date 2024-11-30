/*
 Copyright 2020 CERN for the benefit of the FASER collaboration
*/

#ifndef WAVEWAVEFORMCONTAINERCNV_H
#define WAVEWAVEFORMCONTAINERCNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "RawWaveformContainerCnv_p0.h"

#include "WaveRawEvent/RawWaveformContainer.h"
#include "WaveEventAthenaPool/RawWaveformContainer_p0.h"

// The latest persistent representation
typedef RawWaveformContainer_p0 RawWaveformContainer_PERS;
typedef RawWaveformContainerCnv_p0 RawWaveformContainerCnv_PERS;

typedef T_AthenaPoolCustomCnv< RawWaveformContainer, RawWaveformContainer_PERS > RawWaveformContainerCnvBase;

class RawWaveformContainerCnv : public RawWaveformContainerCnvBase {
  friend class CnvFactory<RawWaveformContainerCnv>;

 public:
  RawWaveformContainerCnv (ISvcLocator* svcloc) : RawWaveformContainerCnvBase(svcloc) {}

 protected:
  virtual RawWaveformContainer_PERS* createPersistent (RawWaveformContainer* transCont);
  virtual RawWaveformContainer* createTransient ();

};

#endif
