/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionFilmParameters_H
#define EmulsionGeoModel_EmulsionFilmParameters_H

#include <string>

class EmulsionDataBase;

class EmulsionFilmParameters {

public:

  // Constructor 
  EmulsionFilmParameters(EmulsionDataBase* rdb);

  double baseThickness() const;
  double baseWidth() const;
  double baseHeight() const;
  double filmThickness() const;
  double filmWidth() const;
  double filmHeight() const;

  std::string baseMaterial() const;
  std::string filmMaterial() const;

 private:
  EmulsionDataBase * m_rdb;

};


#endif // EmulsionGeoModel_EmulsionFilmParameters_H
