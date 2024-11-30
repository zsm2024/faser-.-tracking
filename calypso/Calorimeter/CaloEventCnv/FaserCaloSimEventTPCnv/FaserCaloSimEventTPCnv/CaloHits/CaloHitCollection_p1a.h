/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOHITCOLLECTION_P1A_H
#define CALOHITCOLLECTION_P1A_H

/*

Authors: Davide Costanzo Rob Duxfield
Modified for FASER by D. Casper - change internal unsigned short counters to unsigned long
p2, p3, etc reserved for possible future ATLAS changes

*/

#include <vector>
#include <string>

class CaloHitCollection_p1a
{
 public:
/// Default constructor
  CaloHitCollection_p1a ();
  //private:

  std::vector<float>          m_hit1_meanTime;   //  1 element per string
  std::vector<float>          m_hit1_x0;         //
  std::vector<float>          m_hit1_y0;         //
  std::vector<float>          m_hit1_z0;         //
  std::vector<float>          m_hit1_theta;      //
  std::vector<float>          m_hit1_phi;        //
  std::vector<unsigned long>  m_nHits;           //

  std::vector<unsigned short> m_hitEne_2b;       //  1 element per hit
  std::vector<unsigned short> m_hitLength_2b;    //

  std::vector<unsigned short> m_dTheta;          //  1 element per hit except for first hit in string
  std::vector<unsigned short> m_dPhi;            //

  std::vector<float>          m_hitEne_4b;       //  1 element per hit with  m_hitEne_2b[i] == 2**16

  std::vector<float>          m_hitLength_4b;    //  1 element per hit with  m_hitLength_2b[i] == 2**16

  std::vector<unsigned long>  m_barcode;
  std::vector<unsigned long>  m_mcEvtIndex;
  std::vector<char>           m_evtColl;
  std::vector<unsigned long>  m_nBC;

  std::vector<unsigned long>  m_id;
  std::vector<unsigned long>  m_nId;
};


// inlines

inline
CaloHitCollection_p1a::CaloHitCollection_p1a () {}

#endif
