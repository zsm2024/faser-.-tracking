/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


/**
 * @file FaserSCT_ReClustering.h
 * header file for FaserSCT_ReClustering class
 * @author Paul J. Bell
 * @date Sept 2004
 */


#ifndef FaserSiClusterizationTool_FaserSCT_ReClustering_H
#define FaserSiClusterizationTool_FaserSCT_ReClustering_H

class FaserSCT_ID;
class Identifier;
#include <vector>

///This performs reclustering after identifying dead or noisy channels 
class FaserSCT_ReClustering
{
 public:
    
  FaserSCT_ReClustering();  //constructor
  virtual ~FaserSCT_ReClustering() = default;

  /** called by SCT_ClusteringTool. If some bad channel has broken the 
   * cluster (provided as a list of RDOs) in non-consecutive fragments, just split it.
   */ 
  std::vector<std::vector<Identifier> > recluster(std::vector<std::vector<Identifier>>&, const FaserSCT_ID&);
    
 private:
  typedef std::vector<Identifier> ID_Vector; 
  typedef std::vector<Identifier>::iterator Discont; 
};

#endif // FaserSiClusterizationTool_FaserSCT_ReClustering_H
