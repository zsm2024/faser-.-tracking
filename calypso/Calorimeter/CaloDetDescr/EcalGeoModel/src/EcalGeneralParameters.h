/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EcalGeoModel_EcalGeneralParameters_H
#define EcalGeoModel_EcalGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class EcalDataBase;
class TopLevelPlacements;

class EcalGeneralParameters {

public:

  EcalGeneralParameters(EcalDataBase* rdb);
  ~EcalGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  EcalGeneralParameters(const EcalGeneralParameters &) = delete;
  EcalGeneralParameters & operator=(const EcalGeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
private:

  EcalDataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // EcalGeoModel_EcalGeneralParameters_H
