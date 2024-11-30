#ifndef FASERACTSKALMANFILTER_PERFORMANCEWRITER_H
#define FASERACTSKALMANFILTER_PERFORMANCEWRITER_H

#include "TrackerPrepRawData/FaserSCT_Cluster.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "ResPlotTool.h"
#include "EffPlotTool.h"
#include "SummaryPlotTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "GeneratorObjects/McEventCollection.h"
#include "Acts/Geometry/GeometryContext.hpp"
class TFile;
struct FaserActsRecMultiTrajectory;
using TrajectoriesContainer = std::vector<FaserActsRecMultiTrajectory>;

class PerformanceWriterTool : public AthAlgTool {
public:
  PerformanceWriterTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~PerformanceWriterTool() override =  default;

  StatusCode initialize() override;
  StatusCode finalize() override;

  StatusCode write(const Acts::GeometryContext& geoContext, const TrajectoriesContainer& trajectories);

private:
  std::unique_ptr<const Acts::BoundTrackParameters> extrapolateToReferenceSurface(
      const EventContext& ctx, const HepMC::GenParticle* particle) const;
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {
      this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  SG::ReadHandleKey<McEventCollection> m_mcEventCollectionKey {
      this, "McEventCollection", "BeamTruthEvent"};
  ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool {
      this, "ExtrapolationTool", "FaserActsExtrapolationTool"};
  Gaudi::Property<bool> m_noDiagnostics {this, "noDiagnostics", true, "Set ACTS logging level to INFO and do not run performance writer, states writer or summary writer"};
  Gaudi::Property<std::string> m_filePath{this, "FilePath", "performance_ckf.root"};
  TFile* m_outputFile{nullptr};

  /// Plot tool for residuals and pulls.
  ResPlotTool m_resPlotTool;
  ResPlotTool::ResPlotCache m_resPlotCache;
  /// Plot tool for efficiency
  EffPlotTool m_effPlotTool;
  EffPlotTool::EffPlotCache m_effPlotCache;
  /// Plot tool for track hit info
  SummaryPlotTool m_summaryPlotTool;
  SummaryPlotTool::SummaryPlotCache m_summaryPlotCache;
};

#endif  // FASERACTSKALMANFILTER_PERFORMANCEWRITER_H
