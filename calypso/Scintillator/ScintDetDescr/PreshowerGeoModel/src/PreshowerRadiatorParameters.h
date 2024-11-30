/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerRadiatorParameters_H
#define PreshowerGeoModel_PreshowerRadiatorParameters_H

#include <string>

class PreshowerDataBase;

class PreshowerRadiatorParameters {

public:

  // Constructor 
  PreshowerRadiatorParameters(PreshowerDataBase* rdb);

  // General
  double radiatorThickness() const;
  double radiatorWidth() const;
  double radiatorLength() const;
  std::string radiatorMaterial() const;

 private:
  PreshowerDataBase * m_rdb;

};


#endif // PreshowerGeoModel_PreshowerRadiatorParameters_H
