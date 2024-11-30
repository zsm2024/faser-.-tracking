/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS and FASER
  collaborations
*/

#ifndef FASERACTSKALMANFILTER_ITRACKTRUTHMATCHINGTOOL_H
#define FASERACTSKALMANFILTER_ITRACKTRUTHMATCHINGTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkTrack/Track.h"
#include "xAODTruth/TruthParticle.h"

class ITrackTruthMatchingTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(ITrackTruthMatchingTool, 1, 0);

  virtual std::pair<const xAOD::TruthParticle*, int>
  getTruthParticle(const Trk::Track *track) const = 0;
};

#endif /* FASERACTSKALMANFILTER_ITRACKTRUTHMATCHINGTOOL_H */
