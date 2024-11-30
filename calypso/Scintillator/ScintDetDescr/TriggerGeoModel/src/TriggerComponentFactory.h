/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERGEOMODEL_TRIGGERCOMPONENTFACTORY_H
#define TRIGGERGEOMODEL_TRIGGERCOMPONENTFACTORY_H

#include "TriggerIdentifier.h"
#include <string>

namespace ScintDD{class TriggerDetectorManager;}
class TriggerGeometryManager;
class TriggerMaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class TriggerComponentFactory
{

public:
  TriggerComponentFactory(const std::string & name,
                       ScintDD::TriggerDetectorManager* detectorManager,
                       const TriggerGeometryManager* geometryManager,
                       TriggerMaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  ScintDD::TriggerDetectorManager* m_detectorManager;
  const TriggerGeometryManager* m_geometryManager;
  TriggerMaterialManager* m_materials;

  double epsilon() const;
  virtual ~TriggerComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class TriggerSharedComponentFactory : public TriggerComponentFactory
{

public:
  TriggerSharedComponentFactory(const std::string & name,
                             ScintDD::TriggerDetectorManager* detectorManager,
                             const TriggerGeometryManager* geometryManager,
                             TriggerMaterialManager* materials=nullptr) :
    TriggerComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class TriggerUniqueComponentFactory : public TriggerComponentFactory
{

public:
  TriggerUniqueComponentFactory(const std::string & name,
                             ScintDD::TriggerDetectorManager* detectorManager,
                             const TriggerGeometryManager* geometryManager,
                             TriggerMaterialManager* materials=nullptr) :
    TriggerComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(TriggerIdentifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // TRIGGERGEOMODEL_TRIGGERCOMPONENTFACTORY_H
