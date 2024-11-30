/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoPlateParameters_H
#define VetoGeoModel_VetoPlateParameters_H

#include <string>

class VetoDataBase;

class VetoPlateParameters {

public:

  // Constructor 
  VetoPlateParameters(VetoDataBase* rdb);

  // Barrel General
  int    platePmts() const;
  double plateThickness() const;
  double plateWidth() const;
  double plateLength() const;
  std::string plateMaterial() const;

 private:
  VetoDataBase * m_rdb;

};


#endif // VetoGeoModel_VetoPlateParameters_H
