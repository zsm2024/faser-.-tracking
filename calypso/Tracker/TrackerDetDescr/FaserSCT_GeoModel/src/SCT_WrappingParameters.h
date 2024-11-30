/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SCT_GeoModel_SCT_WrappingParameters_H
#define SCT_GeoModel_SCT_WrappingParameters_H

#include <string>

class SCT_DataBase;

class SCT_WrappingParameters {

public:

  // Constructor 
  SCT_WrappingParameters(SCT_DataBase* rdb);

  // Barrel General
  double wrappingThickness() const;
  double wrappingWidth() const;
  double wrappingLength() const;
  std::string wrappingMaterial() const;

 private:
  SCT_DataBase * m_rdb;

};


#endif // SCT_GeoModel_SCT_WrappingParameters_H
