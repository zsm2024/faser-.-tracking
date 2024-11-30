/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerGeneralParameters_H
#define PreshowerGeoModel_PreshowerGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class PreshowerDataBase;
class TopLevelPlacements;

class PreshowerGeneralParameters {

public:

  PreshowerGeneralParameters(PreshowerDataBase* rdb);
  ~PreshowerGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  PreshowerGeneralParameters(const PreshowerGeneralParameters &) = delete;
  PreshowerGeneralParameters & operator=(const PreshowerGeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.
//   double temperature() const;
//   double biasVoltage() const;
//   double depletionVoltage() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
private:

  PreshowerDataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // SCT_GeoModel_SCT_GeneralParameters_H
