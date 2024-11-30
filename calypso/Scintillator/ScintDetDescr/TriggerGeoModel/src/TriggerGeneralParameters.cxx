/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerGeneralParameters.h"
#include "TriggerDataBase.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "ScintGeoModelUtils/TopLevelPlacements.h"

const double TriggerSAFETY = 0.01 * Gaudi::Units::mm; // Used in some places to make envelopes slightly larger to ensure
                                     // no overlaps due to rounding errors.


TriggerGeneralParameters::TriggerGeneralParameters(TriggerDataBase* rdb)
{
  m_rdb = rdb;
  m_placements = new TopLevelPlacements(m_rdb->topLevelTable());
}


TriggerGeneralParameters::~TriggerGeneralParameters()
{
  delete m_placements;
}


const GeoTrf::Transform3D & 
TriggerGeneralParameters::partTransform(const std::string & partName) const 
{
  return m_placements->transform(partName);
}


bool 
TriggerGeneralParameters::partPresent(const std::string & partName) const
{
  return m_placements->present(partName);
}



//
// General
//
double 
TriggerGeneralParameters::safety() const
{
  return TriggerSAFETY;
}

// Default Conditions. Values should be come form conditions data base. These values provide
// default vlaues if nothing from the conditions database is provided.


// double 
// TriggerGeneralParameters::temperature() const
// {
//   if (m_rdb->conditionsTable()->size() == 0) {
//     return 266.15 * Gaudi::Units::kelvin; // -7 C
//   }
//   return (m_rdb->conditions()->getDouble("TEMPERATURE") + 273.15) * Gaudi::Units::kelvin;
// }


// double 
// SCT_GeneralParameters::biasVoltage() const
// {
//   if (m_rdb->conditionsTable()->size() == 0) {
//     return 100 * Gaudi::Units::volt;
//   }
//   return m_rdb->conditions()->getDouble("BIASVOLT") * Gaudi::Units::volt;
// }

// double 
// SCT_GeneralParameters::depletionVoltage() const
// {
//   if (m_rdb->conditionsTable()->size() == 0) {
//     return 20 * Gaudi::Units::volt;
//   }
//   return m_rdb->conditions()->getDouble("DEPLETIONVOLT") * Gaudi::Units::volt;
// }



  
