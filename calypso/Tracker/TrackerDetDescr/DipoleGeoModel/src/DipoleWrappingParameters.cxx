/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "DipoleGeoModel/DipoleWrappingParameters.h"
#include "DipoleGeoModel/DipoleDataBase.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelKernel/GeoDefinitions.h"

const double DIPOLE_SAFETY = 0.01 * Gaudi::Units::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.


DipoleWrappingParameters::DipoleWrappingParameters(DipoleDataBase* rdb)
{
  m_rdb = rdb;
}


DipoleWrappingParameters::~DipoleWrappingParameters()
{ }

//
// General
//
double 
DipoleWrappingParameters::safety() const
{
  return DIPOLE_SAFETY;
}

std::string
DipoleWrappingParameters::material1() const
{
    return m_rdb->dipoleWrapping()->getString("MATERIAL1");
}

std::string
DipoleWrappingParameters::material2() const
{
    return m_rdb->dipoleWrapping()->getString("MATERIAL2");
}

double
DipoleWrappingParameters::radius() const
{
    return m_rdb->dipoleWrapping()->getDouble("RADIUS");
}

double
DipoleWrappingParameters::thickness1() const
{
    return m_rdb->dipoleWrapping()->getDouble("THICKNESS1");
}

double
DipoleWrappingParameters::thickness2() const
{
    return m_rdb->dipoleWrapping()->getDouble("THICKNESS2");
}


