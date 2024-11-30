/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
//   Source file for class SiliconPropertiesVector for SiPropertiesTool
///////////////////////////////////////////////////////////////////

#include "FaserSiPropertiesTool/FaserSiliconPropertiesVector.h"

namespace Tracker {
  
  FaserSiliconPropertiesVector::FaserSiliconPropertiesVector():
    m_data{},
    m_defaultProperties{}
  {
  }

  FaserSiliconPropertiesVector::~FaserSiliconPropertiesVector() {
  }

  void FaserSiliconPropertiesVector::resize(IdentifierHash::value_type size) {
    m_data.resize(size);
  }

  const Tracker::FaserSiliconProperties& FaserSiliconPropertiesVector::getSiProperties(const IdentifierHash& elementHash) const {
    if (m_data.size()<elementHash) return m_defaultProperties;
    return m_data[elementHash];
  }

  void FaserSiliconPropertiesVector::setConditions(const IdentifierHash& elementHash, const double temperature, const double electricField) {
    if (m_data.size()<elementHash) m_data.resize(elementHash);
    m_data[elementHash].setConditions(temperature, electricField);
  }

} // namespace Tracker
