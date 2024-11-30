/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERCOMPONENTFACTORY_H
#define PRESHOWERGEOMODEL_PRESHOWERCOMPONENTFACTORY_H

#include "PreshowerIdentifier.h"
#include <string>

namespace ScintDD{class PreshowerDetectorManager;}
class PreshowerGeometryManager;
class PreshowerMaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class PreshowerComponentFactory
{

public:
  PreshowerComponentFactory(const std::string & name,
                       ScintDD::PreshowerDetectorManager* detectorManager,
                       const PreshowerGeometryManager* geometryManager,
                       PreshowerMaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  ScintDD::PreshowerDetectorManager* m_detectorManager;
  const PreshowerGeometryManager* m_geometryManager;
  PreshowerMaterialManager* m_materials;

  double epsilon() const;
  virtual ~PreshowerComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class PreshowerSharedComponentFactory : public PreshowerComponentFactory
{

public:
  PreshowerSharedComponentFactory(const std::string & name,
                             ScintDD::PreshowerDetectorManager* detectorManager,
                             const PreshowerGeometryManager* geometryManager,
                             PreshowerMaterialManager* materials=nullptr) :
    PreshowerComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class PreshowerUniqueComponentFactory : public PreshowerComponentFactory
{

public:
  PreshowerUniqueComponentFactory(const std::string & name,
                             ScintDD::PreshowerDetectorManager* detectorManager,
                             const PreshowerGeometryManager* geometryManager,
                             PreshowerMaterialManager* materials=nullptr) :
    PreshowerComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(PreshowerIdentifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // PRESHOWERGEOMODEL_PRESHOWERCOMPONENTFACTORY_H
