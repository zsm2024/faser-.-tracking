/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef TRACKCOUNTS_TRACKCOUNTSALG_H
#define TRACKCOUNTS_TRACKCOUNTSALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"

class FaserSCT_ID;
class TTree;

namespace Tracker
{

class TrackCountsAlg : public AthReentrantAlgorithm, AthHistogramming
{
public:
  TrackCountsAlg(const std::string& name, ISvcLocator* pSvcLocator);

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

  const ServiceHandle<ITHistSvc>& histSvc() const;

private:
  const FaserSCT_ID* m_idHelper;
  SG::ReadHandleKey<TrackCollection> m_trackCollection {this, "TrackCollection", "CLusterFit", "Input track collection name" };

  ServiceHandle<ITHistSvc> m_histSvc;

  mutable TTree* m_tree;
  mutable unsigned int m_time;
  mutable float m_station;
};

inline const ServiceHandle<ITHistSvc>& TrackCountsAlg::histSvc() const {
  return m_histSvc;
}

} // namespace Tracker

#endif // TRACKCOUNTS_TRACKCOUNTSALG_H
