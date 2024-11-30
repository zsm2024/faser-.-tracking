#ifndef FASERACTSKALMANFILTER_GHOSTBUSTERS_H
#define FASERACTSKALMANFILTER_GHOSTBUSTERS_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "TrackClassification.h"


class TTree;
class FaserSCT_ID;

class GhostBusters : public AthReentrantAlgorithm, AthHistogramming {
public:
  GhostBusters(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~GhostBusters() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;
  const ServiceHandle <ITHistSvc> &histSvc() const;

private:
  struct Segment {
  public:
    Segment(const Trk::Track *track, const FaserSCT_ID *idHelper);
    // Segment(const Trk::Track *track, const TrackerSimDataCollection &simDataCollection,
    //         const FaserSCT_ID *idHelper);
    inline int station() const { return m_station; }
    inline Amg::Vector3D position() const { return m_position; }
    inline double x() const { return m_position.x(); }
    inline double y() const { return m_position.y(); }
    inline double z() const { return m_position.z(); }
    inline double chi2() const { return m_chi2; }
    inline size_t size() const { return m_size; }
    inline bool isGhost() const { return m_isGhost; }
    // inline size_t majorityHits() const { return m_particleHitCounts.empty() ? -1 : m_particleHitCounts.front().hitCount; }
    inline const Trk::Track *track() const { return m_track; }
    inline void setGhost() { m_isGhost = true; }
   private:
    // std::vector<ParticleHitCount> m_particleHitCounts {};
    size_t m_size;
    Amg::Vector3D m_position;
    int m_station;
    double m_chi2;
    bool m_isGhost = false;
    const Trk::Track *m_track;
  };

  ServiceHandle <ITHistSvc> m_histSvc;
  SG::WriteHandleKey<TrackCollection> m_outputTrackCollection { this, "OutputCollection", "Segments", "Output track collection name" };
  SG::ReadHandleKey<TrackCollection> m_trackCollection { this, "TrackCollection", "SegmentFit", "Input track collection name" };
  // SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey { this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  DoubleProperty m_xTolerance {this, "xTolerance", 0.5};
  DoubleProperty m_yTolerance {this, "yTolerance", 0.25};

  const FaserSCT_ID *m_idHelper;

  mutable TTree* m_tree;
  mutable unsigned int m_event_number;
  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
  mutable double m_chi2;
  mutable unsigned int m_station;
  // mutable unsigned int m_majorityHits;
  mutable unsigned int m_size;
  mutable bool m_isGhost;
};

inline const ServiceHandle <ITHistSvc> &GhostBusters::histSvc() const {
  return m_histSvc;
}

#endif  // FASERACTSKALMANFILTER_GHOSTBUSTERS_H
