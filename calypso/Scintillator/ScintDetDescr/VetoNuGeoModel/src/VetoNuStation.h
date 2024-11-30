/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETONUGEOMODEL_VETONUSTATION_H
#define VETONUGEOMODEL_VETONUSTATION_H

#include "VetoNuComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class VetoNuIdentifier;
class VetoNuPlate;

class VetoNuStation : public VetoNuUniqueComponentFactory
{

public:
  VetoNuStation(const std::string & name,
             VetoNuPlate* plate,
             ScintDD::VetoNuDetectorManager* detectorManager,
             const VetoNuGeometryManager* geometryManager,
             VetoNuMaterialManager* materials);
  virtual GeoVPhysVol * build(VetoNuIdentifier id);

public:
  int    numPlates()   const {return m_numPlates;}
  double platePitch()  const {return m_platePitch;}
  double thickness()   const {return m_thickness;}
  double width()       const {return m_width;}
  double length()      const {return m_length;}
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  VetoNuPlate*  m_plate;

  int         m_numPlates;
  double      m_platePitch;

  double      m_thickness;
  double      m_width;
  double      m_length;

  double      m_safety;
};

#endif // VETONUGEOMODEL_VETONUSTATION_H
