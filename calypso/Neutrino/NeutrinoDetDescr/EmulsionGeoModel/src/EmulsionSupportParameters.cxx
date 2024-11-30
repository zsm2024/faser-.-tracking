/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EmulsionSupportParameters.h"
#include "EmulsionGeometryManager.h"

#include "EmulsionDataBase.h"

#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "GaudiKernel/SystemOfUnits.h"

#include <cmath>


EmulsionSupportParameters::EmulsionSupportParameters(EmulsionDataBase* rdb)
{
  m_rdb = rdb;
}

std::vector<EmulsionSupportParameters::SupportElement>
EmulsionSupportParameters::supportElements() const
{
  std::vector<SupportElement> result;
  IRDBRecordset_ptr table = m_rdb->emulsionSupportTable();
  if (table.get() == nullptr) return result;

  size_t numElements = table->size();
  for (size_t i = 0; i < numElements; i++)
  {
    const IRDBRecord* element = (*table)[i];
    result.push_back(SupportElement(element->getDouble("DX"),
                                    element->getDouble("DY"),
                                    element->getDouble("DZ"),
                                    element->getDouble("X"),
                                    element->getDouble("Y"),
                                    element->getDouble("Z"),
                                    element->getString("MATERIAL"),
                                    element->getString("LABEL")));
  }

  return result;

}