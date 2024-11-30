/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ECALGEOMODEL_ECALROW_H
#define ECALGEOMODEL_ECALROW_H

#include "EcalComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class EcalIdentifier;
class EcalModule;

class EcalRow : public EcalUniqueComponentFactory
{

public:
  EcalRow(const std::string & name,
             EcalModule* module,
             CaloDD::EcalDetectorManager* detectorManager,
             const EcalGeometryManager* geometryManager,
             EcalMaterialManager* materials);
  virtual GeoVPhysVol * build(EcalIdentifier id);

public:
  int    numModules()    const {return m_numModules;}
  double modulePitch()   const {return m_modulePitch;}
  double moduleStagger() const {return m_moduleStagger;}
  double moduleAngle()   const {return m_moduleAngle;}
  double moduleGap()     const {return m_moduleGap;}
  double height()        const {return m_height;}
  double width()         const {return m_width;}
  double length()        const {return m_length;}
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  EcalModule*  m_module;

  int         m_numModules {2};
  double      m_moduleGap {0.0};
  double      m_moduleAngle {0.0};
  double      m_modulePitch {0.0};
  double      m_moduleStagger {0.0};

  double      m_height {0.0};
  double      m_width {0.0};
  double      m_length {0.0};

  double      m_safety {0.0};
};

#endif // ECALGEOMODEL_ECALROW_H
