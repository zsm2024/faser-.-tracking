/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DipoleGeoModel/DipoleGeneralParameters.h"
#include "DipoleGeoModel/DipoleDataBase.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "TrackerGeoModelUtils/TopLevelPlacements.h"

const double DIPOLE_SAFETY = 0.01 * Gaudi::Units::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.


DipoleGeneralParameters::DipoleGeneralParameters(DipoleDataBase* rdb)
{
  m_rdb = rdb;
  m_placements = new TopLevelPlacements(m_rdb->topLevelTable());
}


DipoleGeneralParameters::~DipoleGeneralParameters()
{
  delete m_placements;
}


const GeoTrf::Transform3D & 
DipoleGeneralParameters::partTransform(const std::string & partName) const 
{
  return m_placements->transform(partName);
}


bool 
DipoleGeneralParameters::partPresent(const std::string & partName) const
{
  return m_placements->present(partName);
}

//
// General
//
double 
DipoleGeneralParameters::safety() const
{
  return DIPOLE_SAFETY;
}

std::string
DipoleGeneralParameters::material() const
{
    return m_rdb->dipoleGeneral()->getString("MATERIAL");
}

double
DipoleGeneralParameters::innerRadius() const
{
    return m_rdb->dipoleGeneral()->getDouble("INNERRAD");
}

double
DipoleGeneralParameters::outerRadius() const
{
    return m_rdb->dipoleGeneral()->getDouble("OUTERRAD");
}

double
DipoleGeneralParameters::shortLength() const
{
    return m_rdb->dipoleGeneral()->getDouble("SHORTLENGTH");
}

double
DipoleGeneralParameters::longLength() const
{
    return m_rdb->dipoleGeneral()->getDouble("LONGLENGTH");
}

