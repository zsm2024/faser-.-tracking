#ifndef FASERSPACEPOINTS_H
#define FASERSPACEPOINTS_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include <string>
#include <atomic>

#include "TTree.h"
#include "TFile.h"

class FaserSCT_ID;
namespace  TrackerDD {
class SCT_DetectorManager;
}


namespace Tracker {

class FaserSpacePoints : public AthReentrantAlgorithm, AthHistogramming {
 public:
  FaserSpacePoints(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSpacePoints() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

  const ServiceHandle<ITHistSvc>& histSvc() const;

 private:
  const FaserSCT_ID* m_idHelper{nullptr};
  const TrackerDD::SCT_DetectorManager* m_detMgr {nullptr};
  bool muon_hit(const TrackerSimDataCollection* simDataMap, Identifier id) const;
  ServiceHandle<ITHistSvc> m_histSvc;

  SG::ReadHandleKey<FaserSCT_SpacePointContainer> m_spacePointContainerKey {
    this, "SpacePoints", "SCT_SpacePointContainer"};
  SG::ReadHandleKey<FaserSiHitCollection> m_siHitCollectionKey {
    this, "FaserSiHitCollection", "SCT_Hits"};
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {
    this, "TrackerSimDataCollection", "SCT_SDO_Map"};

  mutable TTree* m_tree;
  mutable int m_run {0};
  mutable int m_event {0};
  mutable int m_station {0};
  mutable int m_layer {0};
  mutable int m_phi {0};
  mutable int m_eta {0};
  mutable int m_side {0};
  mutable double m_x {0};
  mutable double m_y {0};
  mutable double m_z {0};
  mutable bool m_muon_hit {false};
  mutable double m_tx {0};
  mutable double m_ty {0};
  mutable double m_tz {0};
};

inline const ServiceHandle<ITHistSvc>& FaserSpacePoints::histSvc() const {
  return m_histSvc;
}

}

#endif // FASERSPACEPOINTS_H
