/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerGeoModel_TriggerWrappingParameters_H
#define TriggerGeoModel_TriggerWrappingParameters_H

#include <string>

class TriggerDataBase;

class TriggerWrappingParameters {

public:

  // Constructor 
  TriggerWrappingParameters(TriggerDataBase* rdb);

  // Barrel General
  double wrappingThickness() const;
  double wrappingWidth() const;
  double wrappingLength() const;
  std::string wrappingMaterial() const;

 private:
  TriggerDataBase * m_rdb;

};


#endif // TriggerGeoModel_TriggerWrappingParameters_H
