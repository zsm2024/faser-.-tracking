/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoNuGeoModel_VetoNuStationParameters_H
#define VetoNuGeoModel_VetoNuStationParameters_H

#include <string>

class VetoNuDataBase;

class VetoNuStationParameters {

public:

  // Constructor 
  VetoNuStationParameters(VetoNuDataBase* rdb);


  // General
  int    numPlates() const;
  double platePitch() const;

 private:
  VetoNuDataBase * m_rdb;

};


#endif // VetoNuGeoModel_VetoNuStationParameters_H
