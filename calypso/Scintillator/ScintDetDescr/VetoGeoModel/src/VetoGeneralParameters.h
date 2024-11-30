/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoGeneralParameters_H
#define VetoGeoModel_VetoGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class VetoDataBase;
class TopLevelPlacements;

class VetoGeneralParameters {

public:

  VetoGeneralParameters(VetoDataBase* rdb);
  ~VetoGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  VetoGeneralParameters(const VetoGeneralParameters &) = delete;
  VetoGeneralParameters & operator=(const VetoGeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.
//   double temperature() const;
//   double biasVoltage() const;
//   double depletionVoltage() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
private:

  VetoDataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // SCT_GeoModel_SCT_GeneralParameters_H
