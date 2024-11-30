/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETOGEOMODEL_VETOCOMPONENTFACTORY_H
#define VETOGEOMODEL_VETOCOMPONENTFACTORY_H

#include "VetoIdentifier.h"
#include <string>

namespace ScintDD{class VetoDetectorManager;}
class VetoGeometryManager;
class VetoMaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class VetoComponentFactory
{

public:
  VetoComponentFactory(const std::string & name,
                       ScintDD::VetoDetectorManager* detectorManager,
                       const VetoGeometryManager* geometryManager,
                       VetoMaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  ScintDD::VetoDetectorManager* m_detectorManager;
  const VetoGeometryManager* m_geometryManager;
  VetoMaterialManager* m_materials;

  double epsilon() const;
  virtual ~VetoComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class VetoSharedComponentFactory : public VetoComponentFactory
{

public:
  VetoSharedComponentFactory(const std::string & name,
                             ScintDD::VetoDetectorManager* detectorManager,
                             const VetoGeometryManager* geometryManager,
                             VetoMaterialManager* materials=nullptr) :
    VetoComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class VetoUniqueComponentFactory : public VetoComponentFactory
{

public:
  VetoUniqueComponentFactory(const std::string & name,
                             ScintDD::VetoDetectorManager* detectorManager,
                             const VetoGeometryManager* geometryManager,
                             VetoMaterialManager* materials=nullptr) :
    VetoComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(VetoIdentifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // VETOGEOMODEL_VETOCOMPONENTFACTORY_H
