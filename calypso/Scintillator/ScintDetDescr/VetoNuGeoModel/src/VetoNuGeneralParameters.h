/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoNuGeoModel_VetoNuGeneralParameters_H
#define VetoNuGeoModel_VetoNuGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class VetoNuDataBase;
class TopLevelPlacements;

class VetoNuGeneralParameters {

public:

  VetoNuGeneralParameters(VetoNuDataBase* rdb);
  ~VetoNuGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  VetoNuGeneralParameters(const VetoNuGeneralParameters &) = delete;
  VetoNuGeneralParameters & operator=(const VetoNuGeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.
//   double temperature() const;
//   double biasVoltage() const;
//   double depletionVoltage() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
private:

  VetoNuDataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // SCT_GeoModel_SCT_GeneralParameters_H
