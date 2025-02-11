/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Paul Bell 23/7/04
/* This utility class performs a reclustering of SCT hits necessary
   to take account of dead or noisy channels.
   For now the philosophy is that 
   BAD CHANNELS DO NOT BECOME CLUSTERS
   Later we may want to distinguish between dead and noisy
*/
#include "FaserSCT_ReClustering.h"
// #include "FaserSCT_ClusteringTool/FaserSCT_ClusteringTool.h"
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "GaudiKernel/MsgStream.h"
#include <iostream>
#include <vector>

// Constructor
FaserSCT_ReClustering::FaserSCT_ReClustering()
{}
//--------------------------------------------------------------------------
std::vector<std::vector<Identifier> > FaserSCT_ReClustering::recluster(std::vector<std::vector<Identifier> > &idGroups, const FaserSCT_ID& sctID)
{

  //std::cout << "Running the reclustering method" << std::endl;

  /*
    From SCT_ClusteringTool we have a matrix of identifiers - hits on 
    neighboring strips are grouped togther (vector of identifiers) and
    these are grouped into one big vector (idGroups) for that event.
    Most of these clusters are just single channels and are now 
    excluded if dead/noisy. In the rarer case of several neighbouring
    channels being hit, the cluster gets split up into smaller clusters.
    idGroups is then remade into newidGroups and the original destroyed
  */
   std::vector<ID_Vector> newidGroups;

  //Make a vector of iterators. These will point to any discontinuties
  //in the vectors of identifiers
  std::vector<Discont> discontV;

  //The vectors of identifiers are copied to new vectors of identifiers
  //and the old ones destroyed
  ID_Vector newvector;

  //Need one loop over the group members of idGroups
  std::vector<ID_Vector>::iterator firstGroup = idGroups.begin();
  std::vector<ID_Vector>::iterator lastGroup = idGroups.end();
  for (; firstGroup!= lastGroup; ++firstGroup){

    //Need a second loop over the identifiers within each idGroups member
    std::vector<Identifier>::iterator fst = (*firstGroup).begin();
    std::vector<Identifier>::iterator lst = (*firstGroup).end();
    std::vector<Identifier>::iterator prt;
    //std::cout << "Strip summary " << sctID.strip(*fst) << std::endl;
    int prev = sctID.strip(*fst);

    //ONE/3 Store an iterator pointing to 1st strip
    discontV.clear();
    discontV.push_back((*firstGroup).begin());
    
    //now look at rest of strips
    prt=fst;
    fst++;
    for (; fst!=lst; ++fst){
      //std::cout << "Strip summary cont." << sctID.strip(*fst) << std::endl;
      int current = sctID.strip(*fst);

      //**CHECK STRIPS ARE CONSECUTIVE**

      //TWO/3 If not store an iterator pointing either side of the discontinuity
      if (current != prev +1) {
	discontV.push_back(prt);
	discontV.push_back(fst);
	//std::cout << "start and stop of bad strips " << (sctID.strip(*prt)) << (sctID.strip(*fst)) << std::endl;
	prt=fst;
      }
      prev=current;
    }
    //THREE/3 Store an iterator pointing to last strip
    discontV.push_back((*firstGroup).end()-1);

    /*
      Now we have a vector of pointers discontV to the first and last 
      member of each idGroups member and to any discontinuties within
    */

    std::vector<Discont>::iterator fst_d = discontV.begin();
    //NB: *fst_d refers to the element pointed to by fst_d, and that 
    //element is an iterator!
    
    //Copy the old vector of identifiers into however many new ones
    //are needed. If there were no discontinutities, discontV is
    //of size 2 so we are just making a copy
    for(unsigned int sg=1;sg<discontV.size();sg+=2){
      newvector.clear();
      newvector.assign(*fst_d,(*(fst_d+1))+1);
      newidGroups.push_back(newvector);
      fst_d+=2;
    }
  }
  //delete the old idGroups and return the new one
  idGroups.clear();
  newvector.clear();
  return newidGroups;
}


