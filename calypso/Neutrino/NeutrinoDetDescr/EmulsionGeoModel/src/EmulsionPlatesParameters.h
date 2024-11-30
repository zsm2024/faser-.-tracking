/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionPlatesParameters_H
#define EmulsionGeoModel_EmulsionPlatesParameters_H

#include <string>

class EmulsionDataBase;

class EmulsionPlatesParameters {

public:

  // Constructor 
  EmulsionPlatesParameters(EmulsionDataBase* rdb);


  // General
  double platesWidth() const;
  double platesHeight() const;
  double platesThickness() const;
  double platesX() const;
  double platesY() const;
  double platesZ() const;
  std::string platesMaterial() const;

 private:
  EmulsionDataBase * m_rdb;

};


#endif // EmulsionGeoModel_EmulsionPlatesParameters_H
