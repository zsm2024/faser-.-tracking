/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DipoleGeoModel_DipoleGeneralParameters_H
#define DipoleGeoModel_DipoleGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class DipoleDataBase;
class TopLevelPlacements;

class DipoleGeneralParameters {

public:

  DipoleGeneralParameters(DipoleDataBase* rdb);
  ~DipoleGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  DipoleGeneralParameters(const DipoleGeneralParameters &) = delete;
  DipoleGeneralParameters & operator=(const DipoleGeneralParameters &) = delete;

  // General
  double safety() const;

  //Default conditions.
  // double temperature() const;
  // double biasVoltage() const;
  // double depletionVoltage() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;
  
  std::string material() const;
  double      innerRadius() const;
  double      outerRadius() const;
  double      shortLength() const;
  double      longLength()  const;

private:

  DipoleDataBase * m_rdb;
  TopLevelPlacements * m_placements;
    
};


#endif // DipoleGeoModel_DipoleGeneralParameters_H
