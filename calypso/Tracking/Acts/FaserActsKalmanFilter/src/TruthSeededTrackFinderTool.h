#ifndef FASERACTSKALMANFILTER_TRUTHSEEDEDTRACKFINDERTOOL_H
#define FASERACTSKALMANFILTER_TRUTHSEEDEDTRACKFINDERTOOL_H

#include "Acts/Definitions/Units.hpp"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackFinderTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GeneratorObjects/McEventCollection.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrackerSpacePoint/SpacePointForSeedCollection.h"
#include <string>
#include <vector>

class FaserSCT_ID;
namespace TrackerDD {
class SCT_DetectorManager;
}

using namespace Acts::UnitLiterals;

class TruthSeededTrackFinderTool : public extends<AthAlgTool, ITrackFinderTool> {
public:
  TruthSeededTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TruthSeededTrackFinderTool() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  virtual StatusCode run() override;

  //@todo: this should return a set of bound track parameters
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

  virtual const std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>> spacePoints() const override;

private:
  std::shared_ptr<const Acts::BoundTrackParameters> m_initialTrackParameters;
  std::shared_ptr<const Acts::Surface> m_initialSurface;
  std::shared_ptr<std::vector<IndexSourceLink>> m_sourceLinks {};
  std::shared_ptr<IdentifierLink> m_idLinks {};
  std::shared_ptr<std::vector<Measurement>> m_measurements {};
  std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>> m_spacePoints {};

  const FaserSCT_ID* m_idHelper{nullptr};
  const TrackerDD::SCT_DetectorManager* m_detManager{nullptr};

  static std::vector<Acts::Vector3> map2vector(const std::map<int, Acts::Vector3>& positions, int station);
  static Acts::Vector3 average(const std::vector<Acts::Vector3>& spacePoints);
  static std::pair<Acts::Vector3, Acts::Vector3> linear_fit(const std::vector<Acts::Vector3>& position);
  static std::pair<double, double> momentum2(const std::map<int, Acts::Vector3>& hits, double B=0.57) ;

  SG::ReadHandleKey<SpacePointForSeedCollection>  m_spacePointCollectionKey {
    this, "FaserSpacePointsSeedsName", "Seeds_SpacePointContainer"};
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey {this, "McEventCollection", "BeamTruthEvent"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
      this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};

  Gaudi::Property<bool> m_useTrueInitialParameters {this, "useTrueInitialParameters", true};

  // covariance matrix of measurement
  Gaudi::Property<double> m_covMeas00 {this, "covMeas00", 0.0004};
  Gaudi::Property<double> m_covMeas01 {this, "covMeas01", 0.01};
  Gaudi::Property<double> m_covMeas10 {this, "covMeas10", 0.01};
  Gaudi::Property<double> m_covMeas11 {this, "covMeas11", 0.64};

  // smearing of initial parameters
  Gaudi::Property<double> m_sigmaLoc0 {this, "sigmaLoc0", 0};
  Gaudi::Property<double> m_sigmaLoc1 {this, "sigmaLoc1", 0};
  Gaudi::Property<double> m_sigmaPhi {this, "sigmaPhi", 0};
  Gaudi::Property<double> m_sigmaTheta {this, "sigmaTheta", 0};
  Gaudi::Property<double> m_sigmaP {this, "sigmaP", 0};
  Gaudi::Property<double> m_sigmaTime {this, "sigmaTime", 0};

  // covariance parameters
  Gaudi::Property<double> m_covLoc0 {this, "covLoc0", 1};
  Gaudi::Property<double> m_covLoc1 {this, "covLoc1", 1};
  Gaudi::Property<double> m_covPhi {this, "covPhi", 1};
  Gaudi::Property<double> m_covTheta {this, "covTheta", 1};
  Gaudi::Property<double> m_covQOverP {this, "covQOverP", 1};
  Gaudi::Property<double> m_covTime {this, "covTime", 1};
};

inline const std::shared_ptr<std::vector<Tracker::FaserSCT_SpacePoint*>>
TruthSeededTrackFinderTool::spacePoints() const {
  return m_spacePoints;
}

inline const std::shared_ptr<IdentifierLink>
TruthSeededTrackFinderTool::idLinks() const {
  return m_idLinks;
}


#endif // FASERACTSKALMANFILTER_TRUTHSEEDEDTRACKFINDERTOOL_H
