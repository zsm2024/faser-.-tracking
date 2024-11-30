/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_COMPONENTFACTORY_H
#define FASERSCT_GEOMODEL_SCT_COMPONENTFACTORY_H

#include "SCT_Identifier.h"
#include <string>

namespace TrackerDD{class SCT_DetectorManager;}
class SCT_GeometryManager;
class SCT_MaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class SCT_ComponentFactory
{

public:
  SCT_ComponentFactory(const std::string & name,
                       TrackerDD::SCT_DetectorManager* detectorManager,
                       const SCT_GeometryManager* geometryManager,
                       SCT_MaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  TrackerDD::SCT_DetectorManager* m_detectorManager;
  const SCT_GeometryManager* m_geometryManager;
  SCT_MaterialManager* m_materials;

  double epsilon() const;
  virtual ~SCT_ComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class SCT_SharedComponentFactory : public SCT_ComponentFactory
{

public:
  SCT_SharedComponentFactory(const std::string & name,
                             TrackerDD::SCT_DetectorManager* detectorManager,
                             const SCT_GeometryManager* geometryManager,
                             SCT_MaterialManager* materials=nullptr) :
    SCT_ComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class SCT_UniqueComponentFactory : public SCT_ComponentFactory
{

public:
  SCT_UniqueComponentFactory(const std::string & name,
                             TrackerDD::SCT_DetectorManager* detectorManager,
                             const SCT_GeometryManager* geometryManager,
                             SCT_MaterialManager* materials=nullptr) :
    SCT_ComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(SCT_Identifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // FASERSCT_GEOMODEL_SCT_COMPONENTFACTORY_H
