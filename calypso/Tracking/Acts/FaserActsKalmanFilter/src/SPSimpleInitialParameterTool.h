#ifndef SPSIMPLEINITIALPARAMETERTOOL_H
#define SPSIMPLEINITIALPARAMETERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Definitions/Algebra.hpp"

class FaserSCT_ID;


class SPSimpleInitialParameterTool : public AthAlgTool {
  public:
    SPSimpleInitialParameterTool (const std::string& type, 
	const std::string& name, const IInterface* parent);
    ~SPSimpleInitialParameterTool() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    Acts::BoundTrackParameters getInitialParameters(Acts::Vector3 pos1, Acts::Vector3 pos2, Acts::Vector3 pos3) const;
    Acts::BoundTrackParameters getInitialParameters_2stations(Acts::Vector3 pos1, Acts::Vector3 pos2, Acts::Vector3 pos3) const;
    std::vector<Acts::BoundTrackParameters> getInitialParameters_1station(std::vector<Acts::Vector3> pos1, std::vector<Acts::Vector3> pos2, std::vector<Acts::Vector3> pos3) const;

  private:
};

#endif  // SPSIMPLEINITIALPARAMETERTOOL_H
