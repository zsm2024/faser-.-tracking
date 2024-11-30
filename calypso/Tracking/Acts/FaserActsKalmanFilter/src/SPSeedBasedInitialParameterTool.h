#ifndef SPSEEDBASEDINITIALPARAMETERTOOL_H
#define SPSEEDBASEDINITIALPARAMETERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "Acts/EventData/TrackParameters.hpp"
#include "TrackerSeedFinder/TrackerSeedFinder.h"
#include "TrackerSpacePoint/TrackerSeedCollection.h"
#include "TrackerSpacePoint/TrackerSeed.h"

class FaserSCT_ID;


class SPSeedBasedInitialParameterTool : public AthAlgTool {
  public:
    SPSeedBasedInitialParameterTool (const std::string& type, 
	const std::string& name, const IInterface* parent);
    ~SPSeedBasedInitialParameterTool() = default;

    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    std::vector<Acts::BoundTrackParameters> getInitialParameters() const;
    //Acts::BoundTrackParameters getInitialParameters(std::vector<Identifier> ids) const;

  private:
    const FaserSCT_ID* m_idHelper{nullptr}; 
    SG::ReadHandleKey<Tracker::TrackerSeedCollection>  m_trackerSeedContainerKey{this, "FaserTrackerSeedName", "FaserTrackerSeedCollection", "FaserTrackerSeedCollection"};
};

#endif  // TRUTHBASEDINITIALPARAMETERTOOL_H
