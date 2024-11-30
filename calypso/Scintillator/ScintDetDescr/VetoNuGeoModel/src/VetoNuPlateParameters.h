/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoNuGeoModel_VetoNuPlateParameters_H
#define VetoNuGeoModel_VetoNuPlateParameters_H

#include <string>

class VetoNuDataBase;

class VetoNuPlateParameters {

public:

  // Constructor 
  VetoNuPlateParameters(VetoNuDataBase* rdb);

  // Barrel General
  int    platePmts() const;
  double plateThickness() const;
  double plateWidth() const;
  double plateLength() const;
  std::string plateMaterial() const;

 private:
  VetoNuDataBase * m_rdb;

};


#endif // VetoNuGeoModel_VetoNuPlateParameters_H
