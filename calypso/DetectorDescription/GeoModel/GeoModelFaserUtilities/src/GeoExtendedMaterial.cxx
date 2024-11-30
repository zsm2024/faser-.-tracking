/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelFaserUtilities/GeoExtendedMaterial.h"
#include "GeoModelFaserUtilities/GeoMaterialPropertiesTable.h"

GeoExtendedMaterial::GeoExtendedMaterial(const std::string &Name,
					 double Density,
					 GeoMaterialState State,
					 double Temperature,
					 double Pressure):
  GeoMaterial(Name,Density),
  m_state(State),
  m_temperature(Temperature),
  m_pressure(Pressure),
  m_properties(0)
{
}

GeoExtendedMaterial::~GeoExtendedMaterial()
{
  m_properties->unref();
}

