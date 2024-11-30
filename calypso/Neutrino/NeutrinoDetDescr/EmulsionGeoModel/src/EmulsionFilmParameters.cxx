/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionFilmParameters.h"
#include "EmulsionGeometryManager.h"

#include "EmulsionDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


EmulsionFilmParameters::EmulsionFilmParameters(EmulsionDataBase* rdb)
{
  m_rdb = rdb;
}

double 
EmulsionFilmParameters::baseWidth() const
{
  return m_rdb->emulsionFilm()->getDouble("BASEDX") * Gaudi::Units::mm; 
}

double 
EmulsionFilmParameters::baseHeight() const
{
  return m_rdb->emulsionFilm()->getDouble("BASEDY") * Gaudi::Units::mm; 
}

double 
EmulsionFilmParameters::baseThickness() const
{
  return m_rdb->emulsionFilm()->getDouble("BASEDZ") * Gaudi::Units::mm; 
}

std::string EmulsionFilmParameters::baseMaterial() const
{
  return m_rdb->emulsionFilm()->getString("BASEMAT");
}

double 
EmulsionFilmParameters::filmWidth() const
{
  return m_rdb->emulsionFilm()->getDouble("FILMDX") * Gaudi::Units::mm; 
}

double 
EmulsionFilmParameters::filmHeight() const
{
  return m_rdb->emulsionFilm()->getDouble("FILMDY") * Gaudi::Units::mm; 
}

double 
EmulsionFilmParameters::filmThickness() const
{
  return m_rdb->emulsionFilm()->getDouble("FILMDZ") * Gaudi::Units::mm; 
}

std::string EmulsionFilmParameters::filmMaterial() const
{
  return m_rdb->emulsionFilm()->getString("FILMMAT");
}

