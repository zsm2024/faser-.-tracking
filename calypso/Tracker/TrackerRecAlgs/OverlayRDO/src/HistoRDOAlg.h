#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthHistogramming.h"
#include "TrkTrack/TrackCollection.h"

#include <vector>

class TTree;

class HistoRDOAlg : public AthReentrantAlgorithm, AthHistogramming {
public:
  HistoRDOAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~HistoRDOAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;
  const ServiceHandle <ITHistSvc> &histSvc() const;

private:

  ServiceHandle <ITHistSvc> m_histSvc;

  SG::ReadHandleKey<TrackCollection> m_trackCollection1 { this, "TrackCollection1", "Pos_CKFTrackCollectionWithoutIFT", "Input track collection 1 name" };
  SG::ReadHandleKey<TrackCollection> m_trackCollection2 { this, "TrackCollection2", "Neg_CKFTrackCollectionWithoutIFT", "Input track collection 2 name" };

//   mutable TTree* m_tree;

  mutable TTree* m_tree;

  mutable int m_longTracks1;
  mutable std::vector<double> m_p1;
  mutable std::vector<double> m_theta1;
  mutable std::vector<double> m_phi1;
  mutable std::vector<double> m_x1;
  mutable std::vector<double> m_y1;
  mutable std::vector<double> m_charge1;
  mutable std::vector<double> m_chiSquared1;
  mutable std::vector<double> m_nDoF1;
  mutable double m_pTotPolar1;

  mutable int m_longTracks2;
  mutable std::vector<double> m_p2;
  mutable std::vector<double> m_theta2;
  mutable std::vector<double> m_phi2;
  mutable std::vector<double> m_x2;
  mutable std::vector<double> m_y2;
  mutable std::vector<double> m_charge2;
  mutable std::vector<double> m_chiSquared2;
  mutable std::vector<double> m_nDoF2;
  mutable double m_pTotPolar2;

  mutable double m_yAgreement;
  mutable double m_matchFraction;

};

inline const ServiceHandle <ITHistSvc> &HistoRDOAlg::histSvc() const {
  return m_histSvc;
}
