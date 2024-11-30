/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FaserSCT_GeoModel_SCT_BarrelParameters_H
#define FaserSCT_GeoModel_SCT_BarrelParameters_H

#include <string>
#include <vector>

class SCT_DataBase;

class SCT_BarrelParameters {

public:

  class FrameBoolean
  {
    public:
      FrameBoolean(double width, double height, double thickness, double xPos, double yPos, double zPos, int isStaggered)
      : dx{width}, dy{height}, dz{thickness}, x{xPos}, y{yPos}, z{zPos}, staggered{isStaggered == 1} {}
      double dx;
      double dy;
      double dz;
      double x;
      double y;
      double z;
      bool   staggered;
  };

  // Constructor 
  SCT_BarrelParameters(SCT_DataBase* rdb);

  // Barrel General

  double etaHalfPitch() const;
  double phiHalfPitch() const;
  double depthHalfPitch() const;
  double sideHalfPitch() const;

  int    numLayers() const;
  double layerPitch() const;
  double phiStagger() const;


  // Frame General
  std::string frameMaterial() const;
  double frameWidth() const;
  double frameHeight() const;
  double frameThickness() const;

  // Frame Shape
  std::vector<FrameBoolean> frameBooleanVolumes() const;

private:
  SCT_DataBase * m_rdb;

};


#endif // FaserSCT_GeoModel_SCT_BarrelParameters_H
