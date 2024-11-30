#ifndef FASERACTSKALMANFILTER_MULTITRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_MULTITRACKFINDERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandleKey.h"
#include <string>
#include <vector>

#include "TrackerSpacePoint/FaserSCT_SpacePointContainer.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackFinderTool.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "TrkTrack/TrackCollection.h"

class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}


class MultiTrackFinderTool : public extends<AthAlgTool, ITrackFinderTool> {
public:
  MultiTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~MultiTrackFinderTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run() override;

  virtual const std::shared_ptr<std::vector<Acts::BoundTrackParameters>> initialTrackParameters() const override;
  virtual const std::shared_ptr<const Acts::Surface> initialSurface() const override;
  virtual const std::shared_ptr<std::vector<std::vector<IndexSourceLink>>> sourceLinks() const override;
  virtual const std::shared_ptr<std::vector<IdentifierLink>> idLinks() const override;
  virtual const std::shared_ptr<std::vector<std::vector<Measurement>>> measurements() const override;
  virtual const std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>> spacePoints() const override;
  virtual const std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>> clusters() const override;

  struct Tracklet {
  public:
    Tracklet(const std::vector<Identifier>& ids,
             const std::vector<Acts::GeometryIdentifier>& geoIds,
             const std::vector<double>& positions,
             const Amg::Vector3D& fitPosition,
             const std::vector<const Tracker::FaserSCT_Cluster*>& clusters)
        : m_ids(ids), m_geoIds(geoIds), m_positions(positions), m_fitPosition(fitPosition), m_clusters(clusters) {};

    std::vector<Identifier> ids() const { return m_ids; };
    std::vector<Acts::GeometryIdentifier> geoIds() const { return m_geoIds; }
    std::vector<double> clusterPositions() const { return m_positions; }
    const std::vector<const Tracker::FaserSCT_Cluster*>& clusters() const { return  m_clusters; }
    Amg::Vector3D position() const { return  m_fitPosition; }

  private:
    std::vector<Identifier> m_ids;
    std::vector<Acts::GeometryIdentifier> m_geoIds;
    std::vector<double> m_positions;
    Amg::Vector3D m_fitPosition;
    std::vector<const Tracker::FaserSCT_Cluster*> m_clusters;
  };

  struct ProtoTrack {
  public:
    ProtoTrack(const MultiTrackFinderTool::Tracklet& t1,
               const MultiTrackFinderTool::Tracklet& t2,
               const MultiTrackFinderTool::Tracklet& t3)
        : m_t1(t1), m_t2(t2), m_t3(t3) {}

    Acts::BoundTrackParameters initialTrackParameters(
        double covLoc0, double covLoc1, double covPhi, double covTheta, double covQOverP, double covTime) const {
      Acts::Vector3 dir = m_t2.position() - m_t1.position();
      Acts::Vector3 pos = m_t1.position() - m_t1.position().z()/dir.z() * dir;
      Acts::Vector4 pos4 {pos.x(), pos.y(), pos.z(), 0};
      auto [abs_momentum, charge] = momentum({{1, m_t1.position()}, {2, m_t2.position()}, {3, m_t3.position()}});

      Acts::BoundSymMatrix cov = Acts::BoundSymMatrix::Zero();
      cov(Acts::eBoundLoc0, Acts::eBoundLoc0) = covLoc0;
      cov(Acts::eBoundLoc1, Acts::eBoundLoc1) = covLoc1;
      cov(Acts::eBoundPhi, Acts::eBoundPhi) = covPhi;
      cov(Acts::eBoundTheta, Acts::eBoundTheta) = covTheta;
      cov(Acts::eBoundQOverP, Acts::eBoundQOverP) = covQOverP;
      cov(Acts::eBoundTime, Acts::eBoundTime) = covTime;

      Acts::BoundTrackParameters params =
          Acts::BoundTrackParameters(pos4, dir, abs_momentum, charge, cov);
      return params;
    }

     std::tuple<std::vector<Measurement>, std::vector<IndexSourceLink>, std::map<Index, Identifier>, std::vector<const Tracker::FaserSCT_Cluster*>> run() const {
       const int kSize = 1;
       using ThisMeasurement = Acts::Measurement<Acts::BoundIndices, kSize>;
       std::array<Acts::BoundIndices, kSize> Indices = {Acts::eBoundLoc0};
       std::vector<IndexSourceLink> sourceLinks;
       std::map<Index, Identifier> idLinks;
       std::vector<Measurement> measurements;
       std::vector<const Tracker::FaserSCT_Cluster*> clusters;
       clusters.reserve(m_t1.clusters().size() + m_t2.clusters().size() + m_t3.clusters().size());
       clusters.insert(clusters.end(), m_t1.clusters().begin(), m_t1.clusters().end());
       clusters.insert(clusters.end(), m_t2.clusters().begin(), m_t2.clusters().end());
       clusters.insert(clusters.end(), m_t3.clusters().begin(), m_t3.clusters().end());

       for (const MultiTrackFinderTool::Tracklet& tracklet : {m_t1, m_t2, m_t3}) {
         // FIXME check that ids, geoIds and positions have the same size
         auto ids = tracklet.ids();
         auto geoIds = tracklet.geoIds();
         auto positions = tracklet.clusterPositions();
         for (size_t i = 0; i < tracklet.ids().size(); ++i) {
           idLinks[measurements.size()] = ids[i];
           IndexSourceLink sourceLink(geoIds[i], measurements.size());
           Eigen::Matrix<double, 1, 1> clusterPos {positions[i]};
           Eigen::Matrix<double, 1, 1> clusterCov {0.04 * 0.04,};
           Acts::SourceLink sl{sourceLink};
           ThisMeasurement meas(std::move(sl), Indices, clusterPos, clusterCov);
           sourceLinks.push_back(sourceLink);
           measurements.emplace_back(std::move(meas));
         }
       }
       return std::make_tuple(measurements, sourceLinks, idLinks, clusters);
    }

    double chi2() const {
      return calc_chi2({m_t1.position(), m_t2.position(), m_t3.position()});
    }

  private:
    Tracklet m_t1, m_t2, m_t3;

    static std::pair<double, double> momentum(const std::map<int, Amg::Vector3D>& pos, double B=0.57) {
      Acts::Vector3 vec_l = pos.at(3) - pos.at(1);
      double abs_l = std::sqrt(vec_l.y() * vec_l.y() + vec_l.z() * vec_l.z());
      double t = (pos.at(2).z() - pos.at(1).z()) / (pos.at(3).z() - pos.at(1).z());
      Acts::Vector3 vec_m = pos.at(1) + t * vec_l;
      Acts::Vector3 vec_s = pos.at(2) - vec_m;
      double abs_s = std::sqrt(vec_s.y() * vec_s.y() + vec_s.z() * vec_s.z());
      double p_yz = 0.3 * abs_l * abs_l * B / (8 * abs_s * 1000);
      double charge = vec_s.y() < 0 ? 1 : -1;
      return std::make_pair(p_yz, charge);
    }

    static std::pair<Acts::Vector3, Acts::Vector3> linear_fit(const std::vector<Acts::Vector3>& hits) {
      size_t n_hits = hits.size();
      Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> centers(n_hits, 3);
      for (size_t i = 0; i < n_hits; ++i) centers.row(i) = hits[i];
      Acts::Vector3 origin = centers.colwise().mean();
      Eigen::MatrixXd centered = centers.rowwise() - origin.transpose();
      Eigen::MatrixXd cov = centered.adjoint() * centered;
      Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eig(cov);
      Acts::Vector3 axis = eig.eigenvectors().col(2).normalized();
      return std::make_pair(origin, axis);
    }

    double calc_chi2(const std::vector<Acts::Vector3>& hits) const {
      auto [origin, axis] = linear_fit(hits);
      double chi2 = 0;
      for (const Acts::Vector3& hit : hits) {
        Acts::Vector3 exp = origin + (hit.z() - origin.z()) * axis/axis.z();
        chi2 += (exp.x() - hit.x()) * (exp.x() - hit.x()) + (exp.y() - hit.y()) * (exp.y() - hit.y());
      }
      return chi2;
    }
  };

  struct sort_chi2 {
    inline bool operator() (const ProtoTrack& track1, const ProtoTrack& track2) {
      return (track1.chi2() < track2.chi2());
    }
  };

private:
  std::shared_ptr<std::vector<Acts::BoundTrackParameters>> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<std::vector<IndexSourceLink>>> m_sourceLinks {};
  std::shared_ptr<std::vector<IdentifierLink>> m_idLinks {};
  std::shared_ptr<std::vector<std::vector<Measurement>>> m_measurements {};
  std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>> m_spacePoints {};
  std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>> m_clusters {};

  const FaserSCT_ID* m_idHelper {nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager {nullptr};

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
      this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  SG::ReadHandleKey<TrackCollection> m_trackCollection {
      this, "TrackCollection", "SegmentFit", "Input track collection name" };

  // covariance of the initial parameters
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};
};

inline const std::shared_ptr<std::vector<Acts::BoundTrackParameters>>
MultiTrackFinderTool::initialTrackParameters() const {
  return m_initialTrackParameters;
}

inline const std::shared_ptr<const Acts::Surface>
MultiTrackFinderTool::initialSurface() const {
  return m_initialSurface;
}

inline const std::shared_ptr<std::vector<std::vector<IndexSourceLink>>>
MultiTrackFinderTool::sourceLinks() const {
  return m_sourceLinks;
}

inline const std::shared_ptr<std::vector<IdentifierLink>>
MultiTrackFinderTool::idLinks() const {
  return m_idLinks;
}

inline const std::shared_ptr<std::vector<std::vector<Measurement>>>
MultiTrackFinderTool::measurements() const {
  return m_measurements;
}

inline const std::shared_ptr<std::vector<std::vector<Tracker::FaserSCT_SpacePoint*>>>
MultiTrackFinderTool::spacePoints() const {
  return m_spacePoints;
}

inline const std::shared_ptr<std::vector<std::vector<const Tracker::FaserSCT_Cluster*>>>
MultiTrackFinderTool::clusters() const {
  return m_clusters;
}

#endif // FASERACTSKALMANFILTER_MULTITRACKFINDERTOOL_H
