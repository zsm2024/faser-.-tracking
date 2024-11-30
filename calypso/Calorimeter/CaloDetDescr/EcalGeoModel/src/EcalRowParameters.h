/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EcalGeoModel_EcalRowParameters_H
#define EcalGeoModel_EcalRowParameters_H

#include <string>

class EcalDataBase;

class EcalRowParameters {

public:

  // Constructor 
  EcalRowParameters(EcalDataBase* rdb);


  // General
  int    numModules() const;
  double xGap() const;
  double yawAngle() const;

 private:
  EcalDataBase * m_rdb;

};


#endif // EcalGeoModel_EcalRowParameters_H
