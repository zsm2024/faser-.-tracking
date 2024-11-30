/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerGeoModel_TriggerPlateParameters_H
#define TriggerGeoModel_TriggerPlateParameters_H

#include <string>

class TriggerDataBase;

class TriggerPlateParameters {

public:

  // Constructor 
  TriggerPlateParameters(TriggerDataBase* rdb);

  // Barrel General
  int    platePmts() const;
  double plateThickness() const;
  double plateWidth() const;
  double plateLength() const;
  std::string plateMaterial() const;

 private:
  TriggerDataBase * m_rdb;

};


#endif // TriggerGeoModel_TriggerPlateParameters_H
