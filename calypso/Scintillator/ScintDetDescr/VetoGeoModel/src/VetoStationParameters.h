/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoStationParameters_H
#define VetoGeoModel_VetoStationParameters_H

#include <string>

class VetoDataBase;

class VetoStationParameters {

public:

  // Constructor 
  VetoStationParameters(VetoDataBase* rdb);


  // General
  int    numPlates() const;
  double platePitch() const;

 private:
  VetoDataBase * m_rdb;

};


#endif // VetoGeoModel_VetoStationParameters_H
