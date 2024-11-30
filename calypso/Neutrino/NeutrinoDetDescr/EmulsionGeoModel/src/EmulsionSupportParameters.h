/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionSupportParameters_H
#define EmulsionGeoModel_EmulsionSupportParameters_H

#include <string>
#include <vector>

class EmulsionDataBase;

class EmulsionSupportParameters {

public:

  // Constructor 
  EmulsionSupportParameters(EmulsionDataBase* rdb);


  class SupportElement
  {
    public:
      SupportElement(double width, double height, double thickness, double xPos, double yPos, double zPos, std::string material, std::string name)
      : dx{width}, dy{height}, dz{thickness}, x{xPos}, y{yPos}, z{zPos}, mat{material}, label{name} {}
      double dx;
      double dy;
      double dz;
      double x;
      double y;
      double z;
      std::string mat;
      std::string label;
  };

  // General

  std::vector<SupportElement> supportElements() const;  

 private:
  EmulsionDataBase * m_rdb;

};


#endif // EmulsionGeoModel_EmulsionSupportParameters_H
