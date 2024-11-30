/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSKALMANFILTER_FASERACTSKALMANFILTERALG_H
#define FASERACTSKALMANFILTER_FASERACTSKALMANFILTERALG_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "TrackerSpacePoint/SpacePointForSeedCollection.h"
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "MagFieldElements/FaserFieldCache.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "Identifier/Identifier.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "TrkTrack/TrackCollection.h"
//todo#include "TrajectoryWriterTool.h"


// ACTS
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/TrackFitting/KalmanFitter.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/EventData/TrackContainer.hpp"
#include "Acts/EventData/TrackProxy.hpp"
#include "Acts/EventData/VectorTrackContainer.hpp"
#include "Acts/EventData/VectorMultiTrajectory.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Utilities/Result.hpp"


// PACKAGE
#include "FaserActsGeometry/FASERMagneticFieldWrapper.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "FaserActsKalmanFilter/IndexSourceLink.h"
#include "FaserActsKalmanFilter/ITrackFinderTool.h"
#include "FaserActsTrack.h"
#include "TrackFitterFunction.h"

//#include "ProtoTrackWriterTool.h"
#include "RootTrajectoryStatesWriterTool.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

class EventContext;
class IAthRNGSvc;
class FaserSCT_ID;
class TTree;
class TrackFitterFunction;


namespace ActsExtrapolationDetail {
  class VariantPropagator;
}

namespace TrackerDD {
  class SCT_DetectorManager;
}

using TrackFitterResult = Acts::Result<FaserActsTrackContainer::TrackProxy>;


//class FaserActsKalmanFilterAlg : public AthReentrantAlgorithm {
class FaserActsKalmanFilterAlg : public AthAlgorithm {
public:
  FaserActsKalmanFilterAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserActsKalmanFilterAlg() = default;

  StatusCode initialize() override;
//  StatusCode execute(const EventContext& ctx) const override;
  StatusCode execute() override;
  StatusCode finalize() override;
 
  virtual Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const;

private:
  const FaserSCT_ID* m_idHelper {nullptr};
  std::shared_ptr<TrackFitterFunction> m_fit;
  std::unique_ptr<const Acts::Logger> m_logger;

  Gaudi::Property<std::string> m_actsLogging {this, "ActsLogging", "VERBOSE"};
  std::unique_ptr<Trk::Track> makeTrack(Acts::GeometryContext& tgContext, TrackFitterResult& fitResult, std::vector<const Tracker::FaserSCT_Cluster*> clusters) const;
  std::unique_ptr<Trk::TrackParameters> ConvertActsTrackParameterToATLAS(const Acts::BoundTrackParameters &actsParameter, const Acts::GeometryContext& gctx) const;

  // Read handle for conditions object to get the field cache
  SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

  ToolHandle<ITrackFinderTool> m_trackFinderTool {this, "TrackFinderTool", "TruthTrackFinderTool"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
//todo  ToolHandle<TrajectoryWriterTool> m_trajectoryWriterTool {this, "OutputTool", "TrajectoryWriterTool"};
  ToolHandle<RootTrajectoryStatesWriterTool> m_trajectoryStatesWriterTool {this, "RootTrajectoryStatesWriterTool", "RootTrajectoryStatesWriterTool"};
//  ToolHandle<ProtoTrackWriterTool> m_protoTrackWriterTool {this, "ProtoTrackWriterTool", "ProtoTrackWriterTool"};

  SG::ReadHandleKey<McEventCollection> m_mcEventKey { this, "McEventCollection", "BeamTruthEvent" };
  SG::ReadHandleKey<TrackerSimDataCollection> m_sctMap {this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  const TrackerDD::SCT_DetectorManager* m_detManager{nullptr};

  SG::WriteHandleKey<TrackCollection> m_trackCollection { this, "FaserActsKFTrackCollection", "FaserActsKFTrackCollection", "Output track collection" };
};

#endif
