/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_STATION_H
#define FASERSCT_GEOMODEL_SCT_STATION_H

#include "SCT_ComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class SCT_Identifier;
// class SCT_Plane;
class SCT_Frame;
class SCT_Wrapping;

class SCT_Station : public SCT_UniqueComponentFactory
{

public:
  SCT_Station(const std::string & name,
             SCT_Wrapping* wrapping,
             TrackerDD::SCT_DetectorManager* detectorManager,
             const SCT_GeometryManager* geometryManager,
             SCT_MaterialManager* materials);
  virtual GeoVPhysVol * build(SCT_Identifier id);

public:
  int    numLayers()   const { return m_numLayers; }
  double layerPitch()  const { return m_layerPitch; }
  double width() const {return m_width;}
  double height() const {return m_height;}
  double thickness()      const {return m_thickness;}
 
private:
  SCT_Wrapping* m_wrapping;

  void getParameters();
  virtual const GeoLogVol * preBuild();

  SCT_Frame* m_centralPlane;
  SCT_Frame* m_upstreamPlane;
  SCT_Frame* m_downstreamPlane;

  int    m_numLayers;
  double m_layerPitch;

  double m_width;
  double m_height;
  double m_thickness;

  double m_safety;

};

#endif // SCT_GEOMODEL_SCT_STATION_H
