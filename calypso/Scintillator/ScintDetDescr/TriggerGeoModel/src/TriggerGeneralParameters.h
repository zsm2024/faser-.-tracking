/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerGeoModel_TriggerGeneralParameters_H
#define TriggerGeoModel_TriggerGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class TriggerDataBase;
class TopLevelPlacements;

class TriggerGeneralParameters {

public:

  TriggerGeneralParameters(TriggerDataBase* rdb);
  ~TriggerGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  TriggerGeneralParameters(const TriggerGeneralParameters &) = delete;
  TriggerGeneralParameters & operator=(const TriggerGeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.
//   double temperature() const;
//   double biasVoltage() const;
//   double depletionVoltage() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
private:

  TriggerDataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // TriggerGeoModel_TriggerGeneralParameters_H
