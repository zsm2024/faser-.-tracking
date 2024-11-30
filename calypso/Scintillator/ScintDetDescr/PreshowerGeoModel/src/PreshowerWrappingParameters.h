/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerWrappingParameters_H
#define PreshowerGeoModel_PreshowerWrappingParameters_H

#include <string>

class PreshowerDataBase;

class PreshowerWrappingParameters {

public:

  // Constructor 
  PreshowerWrappingParameters(PreshowerDataBase* rdb);

  // Barrel General
  double wrappingThickness() const;
  double wrappingWidth() const;
  double wrappingLength() const;
  std::string wrappingMaterial() const;

 private:
  PreshowerDataBase * m_rdb;

};


#endif // PreshowerGeoModel_PreshowerWrappingParameters_H
