/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKERSIMDATA_P2_H
#define TRACKERSIMDATA_P2_H

#include <vector>                                                      
#include "GeneratorObjectsTPCnv/HepMcParticleLink_p2.h"
                                                                                                       
class TrackerSimData_p2 {
 public:
  TrackerSimData_p2():m_word(0), m_links{}, m_enDeposits{}  {};
// List of Cnv classes that convert this into SimData objects
  friend class TrackerSimDataCnv_p2;
 private:
  unsigned int m_word; // sim data word
  std::vector<HepMcParticleLink_p2> m_links; // HepMCPLs
  std::vector<float> m_enDeposits; //  energy deposits
};
                                                                                                                                                             
#endif
