/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERSTATION_H
#define PRESHOWERGEOMODEL_PRESHOWERSTATION_H

#include "PreshowerComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class GeoMaterial;
class PreshowerIdentifier;
class PreshowerPlate;
class PreshowerWrapping;

class PreshowerStation : public PreshowerUniqueComponentFactory
{

public:
  PreshowerStation(const std::string & name,
             PreshowerPlate* plate,
             PreshowerWrapping* wrapping,
             ScintDD::PreshowerDetectorManager* detectorManager,
             const PreshowerGeometryManager* geometryManager,
             PreshowerMaterialManager* materials);
  virtual GeoVPhysVol * build(PreshowerIdentifier id);

public:
  int    numPlates()   const {return m_numPlates;}
  double platePitch()  const {return m_platePitch;}
  double thickness()   const {return m_thickness;}
  double width()       const {return m_width;}
  double length()      const {return m_length;}

  double radiatorThickness() const {return m_radiatorThickness;}
  double radiatorWidth() const {return m_radiatorWidth;}
  double radiatorLength() const {return m_radiatorLength;}
  const GeoMaterial* radiatorMaterial() const {return m_radiatorMaterial;}
  
  double absorberThickness() const {return m_absorberThickness;}
  double absorberWidth() const {return m_absorberWidth;}
  double absorberLength() const {return m_absorberLength;}
  const GeoMaterial* absorberMaterial() const {return m_absorberMaterial;}
 
  double airGap() const {return m_airGap;}

private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  PreshowerPlate*  m_plate;
  PreshowerWrapping* m_wrapping;

  int         m_numPlates;
  double      m_platePitch;

  double      m_thickness;
  double      m_width;
  double      m_length;

  double      m_radiatorThickness;
  double      m_radiatorWidth;
  double      m_radiatorLength;

  const GeoMaterial* m_radiatorMaterial;
  const GeoLogVol*   m_radiatorLog;

  double      m_absorberThickness;
  double      m_absorberWidth;
  double      m_absorberLength;

  const GeoMaterial* m_absorberMaterial;
  const GeoLogVol*   m_absorberLog;

  double      m_airGap;

  double      m_safety;
};

#endif // PRESHOWERGEOMODEL_PRESHOWERSTATION_H
