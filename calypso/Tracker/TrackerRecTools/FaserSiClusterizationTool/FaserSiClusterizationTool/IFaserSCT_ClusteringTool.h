/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file IFaserSCT_ClusteringTool.h
 * Header file for the IFaserSCT_ClusteringTool clas
 * @author Paul J. Bell, 2004
 */


#ifndef FASERSICLUSTERIZATIONTOOL_IFASERSCT_CLUSTERINGTOOL_H
#define FASERSICLUSTERIZATIONTOOL_IFASERSCT_CLUSTERINGTOOL_H

// Base class
#include "GaudiKernel/IAlgTool.h"
#include "TrackerRawData/TrackerRawDataCollection.h"
#include "TrackerRawData/FaserSCT_RDORawData.h"

// no forward declare ... typedef
#include "TrackerPrepRawData/FaserSCT_ClusterCollection.h"
#include "GaudiKernel/ToolHandle.h"

class FaserSCT_ID;

class FaserSCT_ChannelStatusAlg;

namespace Tracker {
  ///Interface for SCT clustering algorithms, taking input from RDOs
  class IFaserSCT_ClusteringTool : virtual public IAlgTool 
  {
  public:

    // InterfaceID
    DeclareInterfaceID(IFaserSCT_ClusteringTool, 1, 0);

    virtual ~IFaserSCT_ClusteringTool() = default;

    /** Deprecated method: Clusterize a collection of SCT raw data objects
     * Called from SCT_clusterization algorithm in InDetPrepRawDataFormation, 
     * once per module
     * @param[in] @c RDOs the raw data objects
     * @param[in] @c manager the detector manager
     * @param[in] @c idHelper the SCT helper class  
     * @param[in] @c status  the class providing information on the channel status (good, masked, 
     *            noisy, dead) for removal of bad channels from clustering 
     * @param[in] flag to control if bad channel removal from clusters is done
     */
    virtual Tracker::FaserSCT_ClusterCollection* clusterize(const TrackerRawDataCollection<FaserSCT_RDORawData>& RDOs,
                                              const FaserSCT_ID& idHelper,
                                              const FaserSCT_ChannelStatusAlg* status,
                                              const bool CTBBadChannels) const = 0;

    /** Clusterize a collection of SCT raw data objects
     * Called from SCT_clusterization algorithm in InDetPrepRawDataFormation, 
     * once per module
     * @param[in] @c RDOs the raw data objects
     * @param[in] @c manager the detector manager
     * @param[in] @c idHelper the SCT helper class  
     * @param[in] @c status  the class providing information on the channel status (good/bad) for removal of bad channels from clustering 
     */
    virtual Tracker::FaserSCT_ClusterCollection *clusterize(const TrackerRawDataCollection<FaserSCT_RDORawData>& rdoCollection,
                                              const FaserSCT_ID& idHelper) const = 0;
  };
}
#endif // FASERSICLUSTERIZATIONTOOL_I_FASERSCT_CLUSTERINGTOOL_H
