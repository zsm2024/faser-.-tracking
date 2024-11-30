/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * FaserSCT_DCSStatCondData.h
 * @file header file for data object
 * @author A. Robichaud-Veronneau - 23/02/07
 **/

#ifndef FASERSCT_DCSSTATCONDDATA_H
#define FASERSCT_DCSSTATCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>
#include <vector>
#include <string>

class FaserSCT_DCSStatCondData {
public:
  //constructor
  FaserSCT_DCSStatCondData();

  //destructor
  virtual ~FaserSCT_DCSStatCondData() = default;
  //@name main methods
  //@{
  /// add defect
  void fill(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
  /// remove a defect
  void remove(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
  /// copy all defects to a users vector, the return value is the size
  int output(const CondAttrListCollection::ChanNum& chanNum, std::vector<std::string>& usersVector) const;
  ///
  int output(const CondAttrListCollection::ChanNum & chanNum) const;
  //@}
  
private:
  typedef std::map<CondAttrListCollection::ChanNum, std::vector<std::string> > DCSConditions;
  DCSConditions m_bad_channels;
};

CLASS_DEF( FaserSCT_DCSStatCondData , 167371458 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( FaserSCT_DCSStatCondData, 63295326 );


#endif // FASERSCT_DCSSTATCONDDATA_H
