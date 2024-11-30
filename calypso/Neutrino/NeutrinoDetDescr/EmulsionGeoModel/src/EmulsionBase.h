/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONGEOMODEL_EMULSIONBASE_H
#define EMULSIONGEOMODEL_EMULSIONBASE_H

#include "EmulsionComponentFactory.h"
#include "EmulsionFilm.h"
#include "EmulsionBaseboard.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;

class EmulsionBase: public EmulsionUniqueComponentFactory
{
public:
  EmulsionBase(const std::string & name,
             NeutrinoDD::EmulsionDetectorManager* detectorManager,
             const EmulsionGeometryManager* geometryManager,
             EmulsionMaterialManager* materials);

public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double height()    const {return m_height;}

  virtual GeoVPhysVol * build(EmulsionIdentifier id);
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  double m_thickness;
  double m_width;
  double m_height;
  
  double m_baseThickness;
  double m_baseHeight;
  double m_baseWidth;
  double m_filmThickness;
  double m_filmHeight;
  double m_filmWidth;

  EmulsionBaseboard* m_baseboard;
  EmulsionFilm* m_front;
  EmulsionFilm* m_back;

  GeoTrf::Transform3D * m_frontPos;
  GeoTrf::Transform3D * m_backPos;
  GeoTrf::Translate3D * m_baseboardPos; 

};

#endif // EMULSIONGEOMODEL_EMULSIONBASE_H
