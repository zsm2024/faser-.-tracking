/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

 ///////////////////////////////////////////////////////////////////
// FaserSCT_Cluster.h
//   Header file for class FaserSCT_Cluster
///////////////////////////////////////////////////////////////////
// (c) Faser Detector software, migrate from Athena
///////////////////////////////////////////////////////////////////

#ifndef TRACKERRAWDATA_FASERSCT_CLUSTER_H
#define TRACKERRAWDATA_FASERSCT_CLUSTER_H

// Base class
#include "TrackerPrepRawData/TrackerCluster.h"

#include <vector>

class Identifier;
class MsgStream;

namespace TrackerDD
{
  class SiDetectorElement;
}

namespace Tracker
{
class FaserSiWidth;

class FaserSCT_Cluster : public TrackerCluster {
 public:
  /** Public, Copy, operator=, constructor*/
  FaserSCT_Cluster();
  FaserSCT_Cluster(const FaserSCT_Cluster &);
  FaserSCT_Cluster(FaserSCT_Cluster &&);
  FaserSCT_Cluster &operator=(const FaserSCT_Cluster &);
  FaserSCT_Cluster &operator=(FaserSCT_Cluster &&);

  FaserSCT_Cluster( 
                const Identifier& RDOId,
                const Amg::Vector2D& locpos, 
                const std::vector<Identifier>& rdoList,
                const FaserSiWidth& width,
                const TrackerDD::SiDetectorElement* detEl,
                const Amg::MatrixX& locErrMat
              );

  // For use by tp converter.
  FaserSCT_Cluster(const Identifier& RDOId,
              const Amg::Vector2D& locpos,
              std::vector<Identifier>&& rdoList,
              const FaserSiWidth& width,
              const TrackerDD::SiDetectorElement* detEl,
              Amg::MatrixX&& locErrMat);

    /** dump information about the PRD object. */
    virtual MsgStream&    dump( MsgStream&    stream) const;

    /** dump information about the PRD object. */
    virtual std::ostream& dump( std::ostream& stream) const;

    /** Some information about timing - which strips had 010 and which 011 */
    uint16_t hitsInThirdTimeBin() const;

    void setHitsInThirdTimeBin(uint16_t hitsInThirdTimeBin);
 
    int stripHasHitInThirdTimeBin(int stripNumberWithinCluster) const;
    
    
 private:
    uint16_t m_hitsInThirdTimeBin;
    
      

};

 inline uint16_t FaserSCT_Cluster::hitsInThirdTimeBin() const
 {
   return m_hitsInThirdTimeBin;  
 }
 
 inline void FaserSCT_Cluster::setHitsInThirdTimeBin(uint16_t hitsInThirdTimeBin) 
 {
   m_hitsInThirdTimeBin = hitsInThirdTimeBin;
 }
 

 inline int FaserSCT_Cluster::stripHasHitInThirdTimeBin(int stripNumberWithinCluster) const {
   return stripNumberWithinCluster <= 16 ? (int)((m_hitsInThirdTimeBin >> stripNumberWithinCluster) & 0x1): 0;
   
 }
 


 MsgStream&    operator << (MsgStream& stream,    const FaserSCT_Cluster& prd);
 std::ostream& operator << (std::ostream& stream, const FaserSCT_Cluster& prd);

}
#endif // TRACKERRAWDATA_FASERSCT_CLUSTER_H
