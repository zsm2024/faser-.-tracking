#pragma once

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackerPrepRawData/FaserSCT_ClusterContainer.h"
#include "xAODFaserWaveform/WaveformHitContainer.h"
#include "xAODFaserWaveform/WaveformHit.h"
#include <xAODEventInfo/EventInfo.h>


class FaserSCT_ID;

class SelectRDOAlg : public AthReentrantAlgorithm {
public:
  SelectRDOAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~SelectRDOAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;

private:

  const FaserSCT_ID* m_sctHelper;

  SG::ReadHandleKey<TrackCollection> m_trackCollection { this, "TrackCollection", "CKFTrackCollectionWithoutIFT", "Input track collection name" };
  SG::ReadHandleKey<TrackCollection> m_segmentCollection { this, "SegmentCollection", "SegmentFit", "Track segment collection name" };
  SG::ReadHandleKey<Tracker::FaserSCT_ClusterContainer> m_clusterContainer { this, "ClusterContainer", "SCT_ClusterContainer" };
  SG::ReadHandleKey<xAOD::WaveformHitContainer> m_triggerContainer { this, "TriggerContainer", "TriggerWaveformHits", "Trigger hit container name" };
  SG::WriteHandleKey<TrackCollection> m_outputTrackCollection { this, "OutputTrackCollection", "Pos_CKFTrackCollectionWithoutIFT", "Output track collection name"};

  IntegerProperty   m_minLayers {this, "MinLayers", 7, "Minimum hit layers to accept track"};
  DoubleProperty    m_maxChi2PerDoF {this, "MaxChi2PerDoF", 25.0, "Maximum chi2 per degree of freedom to accept track"};
  DoubleProperty    m_minMomentum {this, "MinMomentumGeV", 50.0, "Minimum momentum in GeV to accept track"};
  DoubleProperty    m_maxMomentum {this, "MaxMomentumGeV", 5000.0, "Maximum momentum in GeV to accept track"};
  DoubleProperty    m_maxRadius {this, "MaxRadiusMm", 95.0, "Maximum radius at first measurement to accept track"};
  IntegerProperty   m_maxSegmentsTotal {this, "MaxSegmentsTotal", 4, "Maximum number of segments in three stations to accept track"};
  IntegerProperty   m_maxSegmentsStation {this, "MaxSegmentsStation", 2, "Maximum number of segments in any single station to accept track"};
  // IntegerProperty   m_maxClustersStation {this, "MaxClustersStation", 9, "Maximum number of clusters in any single station to accept track"};
  DoubleProperty    m_maxTimingCharge {this, "MaxTimingChargePc", 70.0, "Maximum charge in pC recorded by upper or lower timing scintillator"};
  BooleanProperty   m_acceptPositive {this, "AcceptPositive", true, "Accept positive (true) or negative (false) charged tracks"};

};

