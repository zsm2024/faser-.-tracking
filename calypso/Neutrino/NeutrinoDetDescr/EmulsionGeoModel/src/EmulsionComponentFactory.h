/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EMULSIONGEOMODEL_EMULSIONCOMPONENTFACTORY_H
#define EMULSIONGEOMODEL_EMULSIONCOMPONENTFACTORY_H

#include "EmulsionIdentifier.h"
#include <string>

namespace NeutrinoDD{class EmulsionDetectorManager;}
class EmulsionGeometryManager;
class EmulsionMaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class EmulsionComponentFactory
{

public:
  EmulsionComponentFactory(const std::string & name,
                       NeutrinoDD::EmulsionDetectorManager* detectorManager,
                       const EmulsionGeometryManager* geometryManager,
                       EmulsionMaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  NeutrinoDD::EmulsionDetectorManager* m_detectorManager;
  const EmulsionGeometryManager* m_geometryManager;
  EmulsionMaterialManager* m_materials;

  double epsilon() const;
  virtual ~EmulsionComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class EmulsionSharedComponentFactory : public EmulsionComponentFactory
{

public:
  EmulsionSharedComponentFactory(const std::string & name,
                             NeutrinoDD::EmulsionDetectorManager* detectorManager,
                             const EmulsionGeometryManager* geometryManager,
                             EmulsionMaterialManager* materials=nullptr) :
    EmulsionComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class EmulsionUniqueComponentFactory : public EmulsionComponentFactory
{

public:
  EmulsionUniqueComponentFactory(const std::string & name,
                             NeutrinoDD::EmulsionDetectorManager* detectorManager,
                             const EmulsionGeometryManager* geometryManager,
                             EmulsionMaterialManager* materials=nullptr) :
    EmulsionComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(EmulsionIdentifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // EMULSIONGEOMODEL_EMULSIONCOMPONENTFACTORY_H
