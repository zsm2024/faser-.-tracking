/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ECALGEOMODEL_ECALCOMPONENTFACTORY_H
#define ECALGEOMODEL_ECALCOMPONENTFACTORY_H

#include "EcalIdentifier.h"
#include <string>

namespace CaloDD{class EcalDetectorManager;}
class EcalGeometryManager;
class EcalMaterialManager;

class GeoLogVol;
class GeoVPhysVol;


class EcalComponentFactory
{

public:
  EcalComponentFactory(const std::string & name,
                       CaloDD::EcalDetectorManager* detectorManager,
                       const EcalGeometryManager* geometryManager,
                       EcalMaterialManager* materials);

  const std::string & getName() const {return m_name;}

  // utility function to covert int to string
  std::string intToString(int i) const;

protected: 
  CaloDD::EcalDetectorManager* m_detectorManager;
  const EcalGeometryManager* m_geometryManager;
  EcalMaterialManager* m_materials;

  double epsilon() const;
  virtual ~EcalComponentFactory();

private:
  std::string m_name;
  static const double s_epsilon;

};


class EcalSharedComponentFactory : public EcalComponentFactory
{

public:
  EcalSharedComponentFactory(const std::string & name,
                             CaloDD::EcalDetectorManager* detectorManager,
                             const EcalGeometryManager* geometryManager,
                             EcalMaterialManager* materials=nullptr) :
    EcalComponentFactory(name, detectorManager, geometryManager, materials),
    m_physVolume(nullptr)
  {};
  
  GeoVPhysVol * getVolume() {return  m_physVolume;}

protected:
  GeoVPhysVol * m_physVolume;
  virtual GeoVPhysVol * build() = 0;

};

class EcalUniqueComponentFactory : public EcalComponentFactory
{

public:
  EcalUniqueComponentFactory(const std::string & name,
                             CaloDD::EcalDetectorManager* detectorManager,
                             const EcalGeometryManager* geometryManager,
                             EcalMaterialManager* materials=nullptr) :
    EcalComponentFactory(name, detectorManager, geometryManager, materials),
    m_logVolume{nullptr}
  {};

  virtual GeoVPhysVol * build(EcalIdentifier id) = 0;

protected:
  const GeoLogVol * m_logVolume;

  virtual const GeoLogVol * preBuild() = 0;

};

#endif // ECALGEOMODEL_ECALCOMPONENTFACTORY_H
