/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

#ifndef MYCLUSTERS_MYCLUSTERSALG_H
#define MYCLUSTERS_MYCLUSTERSALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"

class TTree;
class FaserSCT_ID;

class MyClustersAlg : public AthReentrantAlgorithm, AthHistogramming {
public:
  MyClustersAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MyClustersAlg() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

  const ServiceHandle<ITHistSvc>& histSvc() const;

private:
  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_clusterContainerKey {
      this, "FaserSCT_ClusterContainer", "SCT_ClusterContainer", "cluster container"};
  ServiceHandle<ITHistSvc> m_histSvc;
  const FaserSCT_ID* m_idHelper {nullptr};
  mutable TTree* m_tree;

  mutable unsigned int m_run;
  mutable unsigned int m_event;
  mutable unsigned int m_station;
  mutable unsigned int m_layer;
  mutable int m_eta;
  mutable int m_phi;
  mutable int m_side;
  mutable double m_lx;
  mutable double m_ly;
  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
};


inline const ServiceHandle<ITHistSvc>& MyClustersAlg::histSvc() const {
  return m_histSvc;
}


#endif  // MYCLUSTERS_MYCLUSTERSALG_H
