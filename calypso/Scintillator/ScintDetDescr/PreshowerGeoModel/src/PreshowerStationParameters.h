/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerStationParameters_H
#define PreshowerGeoModel_PreshowerStationParameters_H

#include <string>

class PreshowerDataBase;

class PreshowerStationParameters {

public:

  // Constructor 
  PreshowerStationParameters(PreshowerDataBase* rdb);


  // General
  int    numPlates() const;
  double platePitch() const;

 private:
  PreshowerDataBase * m_rdb;

};


#endif // PreshowerGeoModel_PreshowerStationParameters_H
