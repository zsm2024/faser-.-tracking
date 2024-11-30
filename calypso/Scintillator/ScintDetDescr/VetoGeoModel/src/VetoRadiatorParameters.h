/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoRadiatorParameters_H
#define VetoGeoModel_VetoRadiatorParameters_H

#include <string>

class VetoDataBase;

class VetoRadiatorParameters {

public:

  // Constructor 
  VetoRadiatorParameters(VetoDataBase* rdb);

  // Barrel General
  double radiatorThickness() const;
  double radiatorWidth() const;
  double radiatorLength() const;
  std::string radiatorMaterial() const;

 private:
  VetoDataBase * m_rdb;

};


#endif // VetoGeoModel_VetoRadiatorParameters_H
