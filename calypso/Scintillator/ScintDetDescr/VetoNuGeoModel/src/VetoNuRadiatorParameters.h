/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoNuGeoModel_VetoNuRadiatorParameters_H
#define VetoNuGeoModel_VetoNuRadiatorParameters_H

#include <string>

class VetoNuDataBase;

class VetoNuRadiatorParameters {

public:

  // Constructor 
  VetoNuRadiatorParameters(VetoNuDataBase* rdb);

  // Barrel General
  double radiatorThickness() const;
  double radiatorWidth() const;
  double radiatorLength() const;
  std::string radiatorMaterial() const;

 private:
  VetoNuDataBase * m_rdb;

};


#endif // VetoNuGeoModel_VetoNuRadiatorParameters_H
