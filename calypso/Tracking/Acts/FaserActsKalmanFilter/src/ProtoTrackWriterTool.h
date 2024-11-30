#ifndef FASERACTSKALMANFILTER_PROTOTRACKWRITERTOOL_H
#define FASERACTSKALMANFILTER_PROTOTRACKWRITERTOOL_H

#include "TrackerSpacePoint/FaserSCT_SpacePoint.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ToolHandle.h"
#include <memory>
#include <string>
#include <vector>

#include "Acts/EventData/TrackParameters.hpp"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsKalmanFilter/ITrackFinderTool.h"
#include "FaserActsKalmanFilter/Measurement.h"

class FaserSCT_ID;
class TFile;
class TTree;

class ProtoTrackWriterTool : public AthAlgTool {
public:
  ProtoTrackWriterTool(const std::string& type, const std::string& name,
                       const IInterface* parent);
  virtual ~ProtoTrackWriterTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  StatusCode write(std::shared_ptr<const Acts::CurvilinearTrackParameters> protoTrackParameters,
                   std::shared_ptr<std::vector<Measurement>> measurements,
                   const Acts::GeometryContext& ctx) const;

private:
  const FaserSCT_ID* m_idHelper{nullptr};

  ToolHandle<ITrackFinderTool> m_trackFinderTool {this, "TrackFinderTool", "TruthTrackFinderTool"};
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  Gaudi::Property<std::string> m_filePath {this, "File", "ProtoTracks.root", "Root file"};
  TFile* m_file;
  TTree* m_params;
  TTree* m_meas;

  mutable int m_run_number;
  mutable int m_event_number;
  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
  mutable double m_px;
  mutable double m_py;
  mutable double m_pz;
  mutable int m_station;
  mutable int m_layer;
  mutable int m_phi;
  mutable int m_eta;
  mutable int m_side;
  mutable double m_meas_eLOC0;
  mutable double m_meas_eLOC1;
};

#endif // FASERACTSKALMANFILTER_PROTOTRACKWRITERTOOL_H
