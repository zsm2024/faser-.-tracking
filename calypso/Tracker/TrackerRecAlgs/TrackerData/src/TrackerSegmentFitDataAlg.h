/*
  Copyright (C) 2022 CERN for the benefit of the FASER collaboration
*/

#ifndef TRACKERDATA_TRACKERSEGMENTFITDATAALG_H
#define TRACKERDATA_TRACKERSEGMENTFITDATAALG_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"

class TTree;
class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}


namespace Tracker {

class TrackerSegmentFitDataAlg : public AthReentrantAlgorithm, AthHistogramming {
public:
  TrackerSegmentFitDataAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~TrackerSegmentFitDataAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;
  const ServiceHandle <ITHistSvc> &histSvc() const;

private:
  SG::ReadHandleKey<TrackCollection> m_trackCollection {this, "TrackCollection", "SegmentFit", "Input track collection name"};

  ServiceHandle <ITHistSvc> m_histSvc;

  const TrackerDD::SCT_DetectorManager *m_detMgr;
  const FaserSCT_ID *m_idHelper;

  mutable TTree* m_tracks;
  mutable TTree* m_hits;

  mutable unsigned int m_run_number;
  mutable unsigned int m_event_number;

  mutable double m_chi2;
  mutable unsigned int m_nDOF;
  mutable unsigned int m_nClusters;
  mutable double m_track_x;
  mutable double m_track_y;
  mutable double m_track_z;
  mutable double m_track_px;
  mutable double m_track_py;
  mutable double m_track_pz;

  mutable unsigned int m_station;
  mutable unsigned int m_plane;
  mutable unsigned int m_module;
  mutable unsigned int m_row;
  mutable unsigned int m_sensor;
  mutable unsigned int m_strip;
  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
};


inline const ServiceHandle <ITHistSvc> &TrackerSegmentFitDataAlg::histSvc() const {
  return m_histSvc;
}

}  // namespace Tracker

#endif  // TRACKERDATA_TRACKERSEGMENTFITDATAALG_H
