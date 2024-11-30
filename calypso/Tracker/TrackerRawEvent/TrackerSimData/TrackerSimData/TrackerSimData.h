/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Simulation data object associated with an TrackerRawData object
 ------------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************
 An object of this class stores the simulation information associated with 
 a RawData object, in two data members. 

 One is an integer, the "simulation data word", in which is packed 
 information summarizing the digitization. Its interpretation may depend on 
 which of the three subdetectors is concerned, but will typically contain 
 bit-flags for "noise", "lost in readout", etc.

 The other consists of a number (normally only one) of  pair<int, float> 
 specifying the barcode and energy (charge) deposited by a charged particle, 
 or rather that part of the charge attributed to the corresponding RDO.
 Encoding and decoding methods will be supplied outside this class.

 The implementation emphasizes compactness in memory, at the expense of speed,
 because objects of this class will be persistent, but will not be accessed
 in real production running. 
 
 ***************************************************************************/

// $Id: TrackerSimData.h,v 1.15 2004-07-08 20:57:03 costanzo Exp $

#ifndef TRACKERSIMDATA_TrackerSimData_H
#define TRACKERSIMDATA_TrackerSimData_H

#include <utility>
#include <vector>
#include "GeneratorObjects/HepMcParticleLink.h"


class TrackerSimData
{
    friend class FaserSCT_SimHelper;
    
public:
    typedef std::pair<HepMcParticleLink, float> Deposit; // A particle link, and the 
    // energy (charge) which its hits contribute to the current RDO.
    TrackerSimData();
    TrackerSimData (const std::vector< Deposit >& deposits, int simDataWord = 0);
    TrackerSimData (std::vector< Deposit >&& deposits, int simDataWord = 0);
    TrackerSimData (const TrackerSimData& other);
    TrackerSimData &operator=(const TrackerSimData& other);
    TrackerSimData &operator=(TrackerSimData&& other);
    virtual ~TrackerSimData();
    int word() const;                           // Get the packed simdata word
    void deposits(std::vector<Deposit>& deposits) const; // Get the Deposits
    const std::vector< Deposit >& getdeposits() const;

private:
    int m_word;   
    //    Deposit* m_p_deposits; but use vector meantime, needs more work
    std::vector<Deposit> m_deposits;
};

inline int TrackerSimData::word() const
{
    return m_word & 0x1fffffff;
}

inline const std::vector< TrackerSimData::Deposit >& TrackerSimData::getdeposits() const
{
  return m_deposits;
}

inline void TrackerSimData::deposits(std::vector< TrackerSimData::Deposit>& deposits) const
{
  // I really don't understand what the point of this is ... it does EXACTLY the same as the getdeposits() method, just in a different way. EJWM
    deposits = m_deposits;
    return;
}

#endif // TRACKERSIMDATA_TrackerSimData_H
