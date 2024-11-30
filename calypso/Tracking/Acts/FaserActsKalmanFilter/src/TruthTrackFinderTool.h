#ifndef FASERACTSKALMANFILTER_TRUTHTRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_TRUTHTRACKFINDERTOOL_H

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackFinderTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include <string>
#include <vector>

class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}


class TruthTrackFinderTool : public extends<AthAlgTool, ITrackFinderTool> {
public:
  TruthTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TruthTrackFinderTool() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run() override;

  virtual const std::shared_ptr<const Acts::BoundTrackParameters> initialTrackParameters() const override {
    return m_initialTrackParameters;
  }

  virtual const std::shared_ptr<const Acts::Surface> initialSurface() const override {
    return m_initialSurface;
  }

  virtual const std::shared_ptr<std::vector<IndexSourceLink>> sourceLinks() const override {
    return m_sourceLinks;
  }

  virtual const std::shared_ptr<IdentifierLink> idLinks() const override;

  virtual const std::shared_ptr<std::vector<Measurement>> measurements() const override {
    return m_measurements;
  }

  virtual const std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint>> spacePoints() const override;

private:
  std::shared_ptr<const Acts::BoundTrackParameters> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<IndexSourceLink>> m_sourceLinks {};
  std::shared_ptr<IdentifierLink> m_idLinks {};
  std::shared_ptr<std::vector<Measurement>> m_measurements {};
  std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>> m_spacePoints {};

  const FaserSCT_ID* m_idHelper{nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager{nullptr};

  SG::ReadHandleKey<FaserSiHitCollection> m_siHitCollectionKey {
    this, "FaserSiHitCollection", "SCT_Hits"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
    this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};

  // smearing of measurements
  Gaudi::Property<double> m_sigma0 {this, "sigmaMeasLoc0", 0};
  Gaudi::Property<double> m_sigma1 {this, "sigmaMeasLoc1", 0};

  // covariance of the measurements
  Gaudi::Property<double> m_covMeasLoc0 {this, "covMeasLoc0", 0.01};
  Gaudi::Property<double> m_covMeasLoc1 {this, "covMeasLoc1", 0.01};

  // smearing of initial parameters
  Gaudi::Property<double> m_sigmaLoc0 {this, "sigmaLoc0", 0};
  Gaudi::Property<double> m_sigmaLoc1 {this, "sigmaLoc1", 0};
  Gaudi::Property<double> m_sigmaPhi {this, "sigmaPhi", 0};
  Gaudi::Property<double> m_sigmaTheta {this, "sigmaTheta", 0};
  Gaudi::Property<double> m_sigmaP {this, "sigmaP", 0};
  Gaudi::Property<double> m_sigmaTime {this, "sigmaTime", 0};

  // covariance of the initial parameters
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};

  // TODO get parameters of first layer from tracker geometry
  Gaudi::Property<int> m_first_station {this, "first_station", 1, "first station for which the initial track parameters are calculated"};
  Gaudi::Property<int> m_first_layer {this, "first_layer", 0, "first layer for which the initial track parameters are calculated"};
  Gaudi::Property<int> m_first_side {this, "first_side", 1, "first side for which the initial track parameters are calculated"};
};

inline const std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>>
    TruthTrackFinderTool::spacePoints() const {
  return m_spacePoints;
}

inline const std::shared_ptr<IdentifierLink>
TruthTrackFinderTool::idLinks() const {
  return m_idLinks;
}


#endif // FASERACTSKALMANFILTER_TRUTHTRACKFINDERTOOL_H
