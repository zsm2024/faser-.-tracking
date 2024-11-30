/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EcalGeneralParameters.h"
#include "EcalDataBase.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "CaloGeoModelUtils/TopLevelPlacements.h"

const double EcalSAFETY = 0.01 * Gaudi::Units::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.


EcalGeneralParameters::EcalGeneralParameters(EcalDataBase* rdb)
{
  m_rdb = rdb;
  m_placements = new TopLevelPlacements(m_rdb->topLevelTable());
}

EcalGeneralParameters::~EcalGeneralParameters()
{
  delete m_placements;
}

const GeoTrf::Transform3D & 
EcalGeneralParameters::partTransform(const std::string & partName) const 
{
  return m_placements->transform(partName);
}

bool 
EcalGeneralParameters::partPresent(const std::string & partName) const
{
  return m_placements->present(partName);
}

//
// General
//
double 
EcalGeneralParameters::safety() const
{
  return EcalSAFETY;
}



  
