/*
  Copyright (C) 2022 CERN for the benefit of the FASER collaboration
*/

#ifndef TRACKERTRUTH_TRACKERTRUTHALG_H
#define TRACKERTRUTH_TRACKERTRUTHALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "GeneratorObjects/McEventCollection.h"

class TTree;
class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}


namespace Tracker {

class TrackerTruthAlg : public AthReentrantAlgorithm, AthHistogramming {
public:
  TrackerTruthAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~TrackerTruthAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;
  const ServiceHandle <ITHistSvc> &histSvc() const;

private:
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey {this, "McEventCollection", "BeamTruthEvent"};
  SG::ReadHandleKey <FaserSiHitCollection> m_faserSiHitKey{this, "FaserSiHitCollection", "SCT_Hits"};

  ServiceHandle <ITHistSvc> m_histSvc;

  const TrackerDD::SCT_DetectorManager *m_sct;
  const FaserSCT_ID *m_sID;

  mutable TTree *m_particles;
  mutable TTree *m_hits;

  mutable unsigned int m_run_number;
  mutable unsigned int m_event_number;
  mutable unsigned int m_station;
  mutable unsigned int m_plane;
  mutable unsigned int m_module;
  mutable unsigned int m_row;
  mutable unsigned int m_sensor;
  mutable int m_pdg;
  mutable double m_local_x;
  mutable double m_local_y;
  mutable double m_global_x;
  mutable double m_global_y;
  mutable double m_global_z;
  mutable double m_vertex_x;
  mutable double m_vertex_y;
  mutable double m_vertex_z;
  mutable double m_px;
  mutable double m_py;
  mutable double m_pz;
};


inline const ServiceHandle <ITHistSvc> &TrackerTruthAlg::histSvc() const {
  return m_histSvc;
}

}  // namespace Tracker

#endif
