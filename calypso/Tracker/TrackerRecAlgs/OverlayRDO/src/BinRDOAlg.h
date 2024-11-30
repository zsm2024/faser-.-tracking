#pragma once

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkTrack/TrackCollection.h"

class BinRDOAlg : public AthReentrantAlgorithm {
public:
  BinRDOAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~BinRDOAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;

private:

  SG::ReadHandleKey<TrackCollection> m_trackCollection { this, "TrackCollection", "CKFTrackCollectionWithoutIFT", "Input track collection name" };
  DoubleProperty     m_xMin {this, "Xmin", -25.0, "Minimum x position to accept"};
  DoubleProperty     m_xMax {this, "Xmax",  25.0, "Maximum x position to accept"};
  DoubleProperty     m_yMin {this, "Ymin", -25.0, "Minimum y position to accept"};
  DoubleProperty     m_yMax {this, "Ymax",  25.0, "Maximum y position to accept"};

};

