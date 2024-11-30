/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerAbsorberParameters_H
#define PreshowerGeoModel_PreshowerAbsorberParameters_H

#include <string>

class PreshowerDataBase;

class PreshowerAbsorberParameters {

public:

  // Constructor 
  PreshowerAbsorberParameters(PreshowerDataBase* rdb);

  // General
  double absorberThickness() const;
  double absorberWidth() const;
  double absorberLength() const;
  std::string absorberMaterial() const;

 private:
  PreshowerDataBase * m_rdb;

};


#endif // PreshowerGeoModel_PreshowerAbsorberParameters_H
