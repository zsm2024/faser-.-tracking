#ifndef TRUTHBASEDINITIALPARAMETERTOOL_H
#define TRUTHBASEDINITIALPARAMETERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "TrackerSimData/TrackerSimDataCollection.h"
#include "SimWriterTool.h"

#include "Acts/EventData/TrackParameters.hpp"


class TruthBasedInitialParameterTool : public AthAlgTool {
 public:
  TruthBasedInitialParameterTool (const std::string& type, 
      const std::string& name, const IInterface* parent);
  ~TruthBasedInitialParameterTool() = default;

  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  Acts::BoundTrackParameters getInitialParameters(std::vector<Identifier> ids) const;

 private:
  SG::ReadHandleKey<TrackerSimDataCollection> m_simDataCollectionKey {this, "TrackerSimDataCollection", "SCT_SDO_Map"};
  ToolHandle<SimWriterTool> m_simWriterTool{this, "OutputTool", "SimWriterTool"};
};

#endif  // TRUTHBASEDINITIALPARAMETERTOOL_H
