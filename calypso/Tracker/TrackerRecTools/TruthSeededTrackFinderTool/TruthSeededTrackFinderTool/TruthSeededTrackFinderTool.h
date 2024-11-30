/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TruthSeededTrackFinderTool_H
#define TruthSeededTrackFinderTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "AthenaKernel/SlotSpecificObj.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "TrkSpacePoint/SpacePoint.h"

#include <mutex>
#include <string>

class SpacePointCollection;
class SpacePointOverlapCollection;
namespace Tracker { 
  class TrackerCluster;
}


namespace Tracker
{

  class TruthSeededTrackFinderTool : public AthAlgTool {

  public:
    /// Constructor 
    TruthSeededTrackFinderTool(const std::string& type, const std::string& name, const IInterface* parent);

    /// Default destructor
    virtual ~TruthSeededTrackFinderTool() = default;

    /// Return interfaceID
    static const InterfaceID& interfaceID();

    /// Initialize
    virtual StatusCode initialize() override;

    /// Finalize
    virtual StatusCode finalize() override;


    /// Convert clusters to space points: SCT_Clusters -> SCT_SpacePoints
    void fillSCT_SpacePointCollection(const SpacePointCollection* clusters1,
                                      SpacePointCollection* clusters2) const;


  private:
    /// @name Cut parameters
    //@{
  };
}
#endif //TrackerSpacePointMakerTool_H
