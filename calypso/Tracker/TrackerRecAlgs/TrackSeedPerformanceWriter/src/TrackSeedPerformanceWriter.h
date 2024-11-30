#ifndef FASERACTSKALMANFILTER_TRACKSEEDPERFORMANCEWRITER_H
#define FASERACTSKALMANFILTER_TRACKSEEDPERFORMANCEWRITER_H


#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackerSimData/TrackerSimDataCollection.h"

class TTree;
class FaserSCT_ID;


namespace Tracker {
  class TrackSeedPerformanceWriter : public AthReentrantAlgorithm, AthHistogramming {
  public:
    TrackSeedPerformanceWriter(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~TrackSeedPerformanceWriter() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;
    virtual StatusCode finalize() override;
    const ServiceHandle<ITHistSvc> &histSvc() const;

  private:
    int matchHit(Identifier id, const TrackerSimDataCollection *simDataCollection) const;
    SG::ReadHandleKey<TrackCollection> m_trackCollectionKey {
        this, "TrackCollection", "SegmentFit", "Input track collection name"};
    SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {
        this, "TrackerSimDataCollection", "SCT_SDO_Map"};
    ServiceHandle<ITHistSvc> m_histSvc;
    const FaserSCT_ID *m_idHelper;
    mutable TTree *m_tree;

    mutable unsigned int m_run;
    mutable unsigned int m_event;
    mutable unsigned int m_station;
    mutable double m_chi2;
    mutable int m_dof;
    mutable int m_nHits;
    mutable double m_x;
    mutable double m_y;
    mutable double m_z;
    mutable double m_px;
    mutable double m_py;
    mutable double m_pz;
    mutable int m_barcode;
    mutable int m_majorityParticle;
    mutable int m_nMajorityHits;
    mutable std::vector<int> m_barcodes;
    mutable std::map<int, int> m_hitCounts;
  };


  inline const ServiceHandle<ITHistSvc> &TrackSeedPerformanceWriter::histSvc() const {
    return m_histSvc;
  }

}  // namespace Tracker


#endif // FASERACTSKALMANFILTER_TRACKSEEDPERFORMANCEWRITER_H
