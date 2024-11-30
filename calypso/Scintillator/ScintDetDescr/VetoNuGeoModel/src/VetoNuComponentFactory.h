/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETONUGEOMODEL_VETONUCOMPONENTFACTORY_H
#define VETONUGEOMODEL_VETONUCOMPONENTFACTORY_H

#include "VetoNuIdentifier.h"
#include <string>

namespace ScintDD{class VetoNuDetectorManager;}
class VetoNuGeometryManager;
class VetoNuMaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class VetoNuComponentFactory
{

public:
  VetoNuComponentFactory(const std::string & name,
                       ScintDD::VetoNuDetectorManager* detectorManager,
                       const VetoNuGeometryManager* geometryManager,
                       VetoNuMaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  ScintDD::VetoNuDetectorManager* m_detectorManager;
  const VetoNuGeometryManager* m_geometryManager;
  VetoNuMaterialManager* m_materials;

  double epsilon() const;
  virtual ~VetoNuComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class VetoNuSharedComponentFactory : public VetoNuComponentFactory
{

public:
  VetoNuSharedComponentFactory(const std::string & name,
                             ScintDD::VetoNuDetectorManager* detectorManager,
                             const VetoNuGeometryManager* geometryManager,
                             VetoNuMaterialManager* materials=nullptr) :
    VetoNuComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class VetoNuUniqueComponentFactory : public VetoNuComponentFactory
{

public:
  VetoNuUniqueComponentFactory(const std::string & name,
                             ScintDD::VetoNuDetectorManager* detectorManager,
                             const VetoNuGeometryManager* geometryManager,
                             VetoNuMaterialManager* materials=nullptr) :
    VetoNuComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(VetoNuIdentifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // VETOGEOMODEL_VETOCOMPONENTFACTORY_H
