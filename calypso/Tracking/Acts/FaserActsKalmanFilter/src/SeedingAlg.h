#ifndef FASERACTSKALMANFILTER_SEEDINGALG_H
#define FASERACTSKALMANFILTER_SEEDINGALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "FaserActsKalmanFilter/ITrackSeedTool.h"
#include <boost/dynamic_bitset.hpp>


class SeedingAlg : public AthAlgorithm {
public:
  SeedingAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~SeedingAlg() = default;

  StatusCode initialize() override;
  StatusCode execute() override;
  StatusCode finalize() override;

private:
  ToolHandle<ITrackSeedTool> m_trackSeedTool {this, "TrackSeed", "CircleFitTrackSeedTool"};
};

#endif // FASERACTSKALMANFILTER_SEEDINGALG_H
