/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_Frame_H
#define FASERSCT_GEOMODEL_SCT_Frame_H

#include "SCT_ComponentFactory.h"
#include "SCT_BarrelParameters.h"

#include <string>

class GeoMaterial;
class GeoVPhysVol;
class SCT_Module;

class SCT_Frame: public SCT_UniqueComponentFactory
{
public:
  SCT_Frame(const std::string & name,
                TrackerDD::SCT_DetectorManager* detectorManager,
                const SCT_GeometryManager* geometryManager,
                SCT_MaterialManager* materials);
  // no copy or assign
  SCT_Frame(const SCT_Frame &) = delete;
  SCT_Frame & operator=(const SCT_Frame &) = delete;

public:
  const GeoMaterial * material() const {return m_material;}
  double thickness() const {return m_thickness;}
  double width()     const {return m_width;}
  double height()    const {return m_height;}
  double stagger()   const {return m_stagger;}
  virtual GeoVPhysVol * build(SCT_Identifier id);

private:

  void getParameters();
  virtual const GeoLogVol* preBuild();
  
  const GeoMaterial * m_material;
  const GeoMaterial * m_air;
  double m_thickness;
  double m_width;
  double m_height;
  double m_stagger;
  double m_etaHalfPitch;
  double m_phiHalfPitch;
  double m_depthHalfPitch;
  double m_sideHalfPitch;

  std::vector<SCT_BarrelParameters::FrameBoolean> m_booleans;

  double m_safety;

  SCT_Module* m_module;
  const GeoLogVol* m_frame;
};

#endif // FASERSCT_GEOMODEL_SCT_Frame_H
