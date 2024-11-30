/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGERGEOMODEL_TRIGGERSTATION_H
#define TRIGGERGEOMODEL_TRIGGERSTATION_H

#include "TriggerComponentFactory.h"

class GeoVPhysVol;
class GeoFullPhysVol;
class GeoLogVol;
class TriggerIdentifier;
class TriggerPlate;
class TriggerWrapping;

class TriggerStation : public TriggerUniqueComponentFactory
{

public:
  TriggerStation(const std::string & name,
             TriggerPlate* plate,
             TriggerWrapping* wrapping,
             ScintDD::TriggerDetectorManager* detectorManager,
             const TriggerGeometryManager* geometryManager,
             TriggerMaterialManager* materials);
  virtual GeoVPhysVol * build(TriggerIdentifier id);

public:
  int    numPlates()   const {return m_numPlates;}
  double platePitchY()  const {return m_platePitchY;}
  double platePitchZ()  const {return m_platePitchZ;}
  double thickness()   const {return m_thickness;}
  double width()       const {return m_width;}
  double length()      const {return m_length;}
 
private:
  void getParameters();
  virtual const GeoLogVol * preBuild();
 
  TriggerPlate*  m_plate;
  TriggerWrapping* m_wrapping;

  int         m_numPlates;
  double      m_platePitchY;
  double      m_platePitchZ;

  double      m_thickness;
  double      m_width;
  double      m_length;

  double      m_safety;
};

#endif // TRIGGERGEOMODEL_TRIGGERSTATION_H
