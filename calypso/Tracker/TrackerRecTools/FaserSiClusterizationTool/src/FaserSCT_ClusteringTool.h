/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
   */

/** @file FaserSCT_ClusteringTool.h
 *  Header file for FaserSCT_ClusteringTool
 *
 */
#ifndef FaserSiClusterizationTool_FaserSCT_ClusteringTool_H
#define FaserSiClusterizationTool_FaserSCT_ClusteringTool_H

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "Identifier/Identifier.h"
//#include "FaserSCT_ConditionsTools/ISCT_DCSConditionsTool.h"
#include "FaserSCT_ConditionsTools/ISCT_ConditionsSummaryTool.h"
#include "InDetCondTools/ISiLorentzAngleTool.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "FaserSiClusterizationTool/IFaserSCT_ClusteringTool.h"
#include "TrackerClusterMakerTool.h"
#include "StoreGate/ReadCondHandleKey.h"
//Gaudi
#include "GaudiKernel/ToolHandle.h"
//STL
#include <bitset>
#include <string>
#include <vector>

class FaserSCT_ID;
class FaserSCT_ChannelStatusAlg;
class StatusCode;

namespace TrackerDD{
  class SCT_ModuleSideDesign;
}

namespace Tracker
{
/**  AlgTool for FaserSCT_Clusterization.
 * Input is from RDOs, assumed to be sorted. They are then scanned 
 * in order and neighbouring RDOs are grouped together.
 */
    class FaserSCT_ClusteringTool: public extends<AthAlgTool, IFaserSCT_ClusteringTool> {
      public:
        ///Normal constructor for an AlgTool; 'properties' are also declared here
        FaserSCT_ClusteringTool(const std::string& type, const std::string& name, const IInterface* parent);
        ///Retrieve the necessary services in initialize                
        StatusCode initialize();
        virtual ~FaserSCT_ClusteringTool() = default;

        /// Clusterize the SCT RDOs... deprecated form passes explicit channel status object
        virtual Tracker::FaserSCT_ClusterCollection*
          clusterize(const TrackerRawDataCollection<FaserSCT_RDORawData>& RDOs,
                     const FaserSCT_ID& idHelper,
                     const FaserSCT_ChannelStatusAlg* status,
                     const bool CTBBadChannels) const;

        /// Clusterize the SCT RDOs...
        virtual Tracker::FaserSCT_ClusterCollection*
          clusterize(const TrackerRawDataCollection<FaserSCT_RDORawData>& RDOs,
        const FaserSCT_ID& idHelper) const;

        /// Clusterize the SCT RDOs... new method originally implemented for ITk
        virtual Tracker::FaserSCT_ClusterCollection*
          clusterizeNew(const TrackerRawDataCollection<FaserSCT_RDORawData>& RDOs,
        const FaserSCT_ID& idHelper) const;

      private:
        IntegerProperty m_errorStrategy{this, "errorStrategy", 1};
        BooleanProperty m_checkBadChannels{this, "checkBadChannels", false};
        // ToolHandle<ISCT_DCSConditionsTool>          m_conditionsTool{this, "FaserSCT_DCSConditionsTool", "FaserSCT_ConditionsTools", "Tool to retrieve SCT Conditions summary"};
        const FaserSCT_ID* m_pHelper; //!< ID helper for FaserSCT

        ToolHandle<ISCT_ConditionsSummaryTool> m_conditionsTool{this, "conditionsTool", "FaserSCT_ConditionsTools/FaserSCT_ConditionsSummaryTool", "Tool to retrieve SCT Conditions summary"};
        ToolHandle< TrackerClusterMakerTool >            m_clusterMaker{this, "globalPosAlg", "TrackerClusterMakerTool"};
        ToolHandle<ISiLorentzAngleTool> m_lorentzAngleTool
        {this, "FaserSiLorentzAngleTool", "FaserSiLorentzAngleTool", "Tool to retreive Lorentz angle of SCT"};
        typedef std::vector<Identifier>           IdVec_t;
        StringProperty m_timeBinStr{this, "timeBins", "01X"};
        int m_timeBinBits[3]{-1, -1, -1};
        // BooleanProperty m_innermostBarrelX1X{this, "innermostBarrelX1X", false};
        // BooleanProperty m_innertwoBarrelX1X{this, "innertwoBarrelX1X", false};
        // BooleanProperty m_majority01X{this, "majority01X", false};
        BooleanProperty m_useRowInformation{this, "useRowInformation", false};
        BooleanProperty m_doNewClustering{this, "doNewClustering", false};

        SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};


        ///Add strips to a cluster vector without checking for bad strips
        void  addStripsToCluster(const Identifier& firstStripId, unsigned int nStrips, IdVec_t& clusterVector, const FaserSCT_ID& idHelper) const;

        ///Add strips to a cluster vector checking for bad strips
        void addStripsToClusterWithChecks(const Identifier& firstStripId, unsigned int nStrips, IdVec_t& clusterVector,
          std::vector<IdVec_t>& idGroups, const FaserSCT_ID& idHelper) const;

        void addStripsToClusterInclRows(const Identifier& firstStripId, unsigned int nStrips, IdVec_t& clusterVector, std::vector<IdVec_t>& idGroups, const FaserSCT_ID& idHelper) const;

        /** Recluster the current vector, splitting on bad strips, and insert those new groups to the idGroups vector.
         * The cluster vector referenced will be changed by this, as well as the idGroups
         **/
        IdVec_t recluster(IdVec_t& clusterVector, std::vector<IdVec_t>& idGroups) const;

        /// In-class struct to store the centre and width of a cluster                                
        struct DimensionAndPosition {
          TrackerDD::SiLocalPosition centre;
          double width;
          DimensionAndPosition(const TrackerDD::SiLocalPosition& c, double w):centre(c), width(w){}
        };

        ///Calculate the cluster position and width given the first and last strip numbers for this element
        DimensionAndPosition clusterDimensions(int firstStrip, int lastStrip, const TrackerDD::SiDetectorElement* element,
          const FaserSCT_ID& idHelper) const;

        DimensionAndPosition clusterDimensionsInclRow(int firstStrip, int lastStrip, int row, const TrackerDD::SiDetectorElement* element, const TrackerDD::SCT_ModuleSideDesign* design) const;

        /// In-class facade on the 'isGood' method for a strip identifier
        bool isBad(const Identifier& stripId) const;

        // Convert time bin string to array of 3 bits
        StatusCode decodeTimeBins();
        // Convert a single time bin char to an int, bit is modified
        StatusCode decodeTimeBin(char timeBin, int& bit) const;
        // Test the clusters time bin to see if matches pattern
        bool       testTimeBins(int timeBin) const;
        bool       testTimeBins01X(int timeBin) const;
        bool       testTimeBinsX1X(int timeBin) const;
        bool       testTimeBinsN(const std::bitset<3>& timePattern) const;
    };//end of class  

    ///////////////////////////////////////////////////////////////////
    // Inline methods
    ///////////////////////////////////////////////////////////////////

    inline bool FaserSCT_ClusteringTool::isBad(const Identifier& stripId) const {
     return (not m_conditionsTool->isGood(stripId, InDetConditions::SCT_STRIP));
    }

    inline bool FaserSCT_ClusteringTool::testTimeBinsN(const std::bitset<3>& timePattern) const {
      // Convert the given timebin to a bit set and test each bit
      // if bit is -1 (i.e. X) it always passes, other wise require exact match of 0/1
      // N.B bitset has opposite order to the bit pattern we define

      if (m_timeBinBits[0] != -1 and timePattern.test(2) != static_cast<bool>(m_timeBinBits[0])) return false;
      if (m_timeBinBits[1] != -1 and timePattern.test(1) != static_cast<bool>(m_timeBinBits[1])) return false;
      if (m_timeBinBits[2] != -1 and timePattern.test(0) != static_cast<bool>(m_timeBinBits[2])) return false;
      return true;
    }

}
#endif // FaserSiClusterizationTool_FaserSCT_ClusteringTool_H
