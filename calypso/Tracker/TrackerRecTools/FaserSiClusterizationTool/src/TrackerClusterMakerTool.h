/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//ClusterMaker.h
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Fill the global position fields of the PrepRawData
///////////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////////////////
// First version 04/08/2003 Tommaso Lari
//
///////////////////////////////////////////////////////////////////

#ifndef FASERSCT_CLUSTERINGTOOL_TRACKERCLUSTERMAKERTOOL_H
#define FASERSCT_CLUSTERINGTOOL_TRACKERCLUSTERMAKERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"

#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ToolHandle.h"

#include <atomic>
#include <vector>

#include "TrackerReadoutGeometry/SiDetectorElement.h"

template <class T> class ServiceHandle;
class Identifier;
class StatusCode;

namespace TrackerDD {
  class SiDetectorElement;
}


static const InterfaceID IID_TrackerClusterMakerTool("TrackerClusterMakerTool", 1, 0);
namespace Tracker 
{
  class FaserSCT_Cluster;
  class FaserSiWidth;

  class TrackerClusterMakerTool : public AthAlgTool {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////
  public:

    TrackerClusterMakerTool(const std::string &type,
        const std::string &name,
        const IInterface *parent);
    ~TrackerClusterMakerTool() = default;
    
    static const InterfaceID& interfaceID() { return IID_TrackerClusterMakerTool; };

    StatusCode initialize();
    
  // Computes global position and errors for SCT cluster.
  // Called by SCT Clustering tools
  // 
  // Input parameters
  // - the cluster Identifier 
  // - the position in local reference frame 
  // - the list of identifiers of the Raw Data Objects belonging to the cluster
  // - the width of the cluster
  // - the module the cluster belongs to  
  // - the error strategy, currently
  //    0: Cluster Width/sqrt(12.)
  //    1: Set to a different values for one and two-strip clusters (def.)

    Tracker::FaserSCT_Cluster* sctCluster(const Identifier& clusterID,
                          const Amg::Vector2D& localPos,
                          const std::vector<Identifier>& rdoList,
                          const Tracker::FaserSiWidth& width,
                          const TrackerDD::SiDetectorElement* element,
                          int errorStrategy) const;
  
  private:



  //  ToolHandle<FaserSiLorentzAngleTool> m_sctLorentzAngleTool
  //  {this, "FaserSCTLorentzAngleTool", "FaserSiLorentzAngleTool/FaserSiLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};
    ToolHandle<ISiLorentzAngleTool> m_sctLorentzAngleTool
      {this, "FaserSiLorentzAngleTool", "FaserSiLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};  

    // These std::atomic_bool may be dropped.
    // m_issueErrorA and m_issueErrorB are changed in pixelCluster but do not affect any computation.
    // The default values of m_forceErrorStrategy1A and m_forceErrorStrategy1B are unchanged.
    // If they are changed in event processing and affect some computation, they are not thread-safe.
    mutable std::atomic_bool m_issueErrorA{true};
    mutable std::atomic_bool m_forceErrorStrategy1A{false};
    mutable std::atomic_bool m_issueErrorB{true};
    mutable std::atomic_bool m_forceErrorStrategy1B{false};
  };

}
#endif // TRACKER_TRACKERCLUSTERMAKERTOOL_H
