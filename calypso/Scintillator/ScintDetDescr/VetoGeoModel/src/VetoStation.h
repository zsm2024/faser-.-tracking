/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VETOGEOMODEL_VETOSTATION_H
#define VETOGEOMODEL_VETOSTATION_H

#include "VetoComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class VetoIdentifier;
class VetoPlate;
class VetoWrapping;

class VetoStation : public VetoUniqueComponentFactory
{

public:
  VetoStation(const std::string & name,
             VetoPlate* plate,
             VetoWrapping* wrapping,
             ScintDD::VetoDetectorManager* detectorManager,
             const VetoGeometryManager* geometryManager,
             VetoMaterialManager* materials);
  virtual GeoVPhysVol * build(VetoIdentifier id);

public:
  int    numPlates()   const {return m_numPlates;}
  double platePitch()  const {return m_platePitch;}
  double thickness()   const {return m_thickness;}
  double width()       const {return m_width;}
  double length()      const {return m_length;}
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  VetoPlate*  m_plate;
  VetoWrapping* m_wrapping;

  int         m_numPlates;
  double      m_platePitch;

  double      m_thickness;
  double      m_width;
  double      m_length;

  double      m_safety;
};

#endif // VETOGEOMODEL_VETOSTATION_H
