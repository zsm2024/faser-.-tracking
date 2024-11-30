/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerPlateParameters_H
#define PreshowerGeoModel_PreshowerPlateParameters_H

#include <string>

class PreshowerDataBase;

class PreshowerPlateParameters {

public:

  // Constructor 
  PreshowerPlateParameters(PreshowerDataBase* rdb);

  // Barrel General
  int    platePmts() const;
  double plateThickness() const;
  double plateWidth() const;
  double plateLength() const;
  std::string plateMaterial() const;

 private:
  PreshowerDataBase * m_rdb;

};


#endif // PreshowerGeoModel_PreshowerPlateParameters_H
