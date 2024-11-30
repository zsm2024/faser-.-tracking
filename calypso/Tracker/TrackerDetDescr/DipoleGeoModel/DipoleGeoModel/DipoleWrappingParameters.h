/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DipoleGeoModel_DipoleWrappingParameters_H
#define DipoleGeoModel_DipoleWrappingParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class DipoleDataBase;

class DipoleWrappingParameters {

public:

  DipoleWrappingParameters(DipoleDataBase* rdb);
  ~DipoleWrappingParameters();
  //Explicitly disallow copy, assignment to appease coverity
  DipoleWrappingParameters(const DipoleWrappingParameters &) = delete;
  DipoleWrappingParameters & operator=(const DipoleWrappingParameters &) = delete;

  // General
  double safety() const;

  std::string material1() const;
  std::string material2() const;
  double      radius() const;
  double      thickness1() const;
  double      thickness2() const;
private:

  DipoleDataBase * m_rdb;
    
};


#endif // DipoleGeoModel_DipoleWrappingParameters_H
