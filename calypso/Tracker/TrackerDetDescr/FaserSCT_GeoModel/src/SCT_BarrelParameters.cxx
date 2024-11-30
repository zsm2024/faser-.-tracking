/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_BarrelParameters.h"
#include "SCT_GeometryManager.h"

#include "SCT_DataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


SCT_BarrelParameters::SCT_BarrelParameters(SCT_DataBase* rdb)
{
  m_rdb = rdb;
}


//
// Barrel Layer
//

  double
  SCT_BarrelParameters::etaHalfPitch() const
  {
    return m_rdb->brlGeneral()->getDouble("ETAHALFPITCH");
  }

  double
  SCT_BarrelParameters::phiHalfPitch() const
  {
    return m_rdb->brlGeneral()->getDouble("PHIHALFPITCH");
  }

  double
  SCT_BarrelParameters::depthHalfPitch() const
  {
    return m_rdb->brlGeneral()->getDouble("DEPTHHALFPITCH");
  }

  double
  SCT_BarrelParameters::sideHalfPitch() const
  {
    return m_rdb->brlGeneral()->getDouble("SIDEHALFPITCH");
  }

  double
  SCT_BarrelParameters::phiStagger() const
  {
    return m_rdb->brlGeneral()->getDouble("PHISTAGGER");
  }

//
// Barrel General
//
int
SCT_BarrelParameters::numLayers() const
{
  return m_rdb->brlGeneral()->getInt("NUMLAYERS"); 
}

double
SCT_BarrelParameters::layerPitch() const
{
  return m_rdb->brlGeneral()->getDouble("LAYERPITCH");
}

//
// Frame General
//

std::string
SCT_BarrelParameters::frameMaterial() const
{
  return m_rdb->frameGeneral()->getString("MATERIAL");
}

double
SCT_BarrelParameters::frameWidth() const
{
  return m_rdb->frameGeneral()->getDouble("WIDTH");
}

double
SCT_BarrelParameters::frameHeight() const
{
  return m_rdb->frameGeneral()->getDouble("HEIGHT");
}

double
SCT_BarrelParameters::frameThickness() const
{
  return m_rdb->frameGeneral()->getDouble("THICKNESS");
}

//
// Frame Shape
//

std::vector<SCT_BarrelParameters::FrameBoolean>
SCT_BarrelParameters::frameBooleanVolumes() const
{
  std::vector<FrameBoolean> result;
  IRDBRecordset_ptr table = m_rdb->frameShapeTable();
  if (table.get() == nullptr) return result;

  size_t numBoolean = table->size();
  for (size_t i = 0; i < numBoolean; i++)
  {
    const IRDBRecord* volume = (*table)[i];
    result.push_back(FrameBoolean(volume->getDouble("DX"),
                                  volume->getDouble("DY"),
                                  volume->getDouble("DZ"),
                                  volume->getDouble("X"),
                                  volume->getDouble("Y"),
                                  volume->getDouble("Z"),
                                  volume->getInt("STAGGERED")));
  }

  return result;
}
