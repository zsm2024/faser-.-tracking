/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoWrappingParameters_H
#define VetoGeoModel_VetoWrappingParameters_H

#include <string>

class VetoDataBase;

class VetoWrappingParameters {

public:

  // Constructor 
  VetoWrappingParameters(VetoDataBase* rdb);

  // Barrel General
  double wrappingThickness() const;
  double wrappingWidth() const;
  double wrappingLength() const;
  std::string wrappingMaterial() const;

 private:
  VetoDataBase * m_rdb;

};


#endif // VetoGeoModel_VetoWrappingParameters_H
