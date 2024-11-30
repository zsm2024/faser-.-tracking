/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONGEOMODEL_EMULSIONPLATES_H
#define EMULSIONGEOMODEL_EMULSIONPLATES_H

#include "EmulsionComponentFactory.h"
#include "EmulsionSupportParameters.h"
#include "EmulsionBase.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelKernel/GeoDefinitions.h"

#include <atomic>
#include <string>

class GeoMaterial;
class GeoVPhysVol;

class EmulsionPlates: public EmulsionUniqueComponentFactory
{
public:
  EmulsionPlates(const std::string & name,
             NeutrinoDD::EmulsionDetectorManager* detectorManager,
             const EmulsionGeometryManager* geometryManager,
             EmulsionMaterialManager* materials);

public:
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double height()    const {return m_height;}
  double shift()     const {return m_zShift;}

  virtual GeoVPhysVol * build(EmulsionIdentifier id);
  
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  double m_thickness;
  double m_width;
  double m_height;
  double m_tungstenThickness;
  double m_tungstenWidth;
  double m_tungstenHeight;
  double m_zShift;
  const GeoMaterial * m_absorberMaterial;
  const GeoMaterial * m_airMaterial;

  int m_nModules;
  int m_nBasesPerModule;
  double m_firstBaseZ;
  double m_lastBaseZ;

  std::vector<EmulsionSupportParameters::SupportElement> m_supportElements;

  EmulsionBase* m_base;
  GeoLogVol* m_absorberVolume;
  std::vector<GeoLogVol*> m_supportVolumes;

};

#endif // EMULSIONGEOMODEL_EMULSIONPLATES_H
