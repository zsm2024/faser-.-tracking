/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSIPROPERTIESTOOL_FASERSILICONPROPERTIESVECTOR_H
#define FASERSIPROPERTIESTOOL_FASERSILICONPROPERTIESVECTOR_H

///////////////////////////////////////////////////////////////////
//   Header file for class SiliconPropertiesVector for SiPropertiesTool
///////////////////////////////////////////////////////////////////

#include <vector>
#include "FaserSiPropertiesTool/FaserSiliconProperties.h"
#include "Identifier/IdentifierHash.h"
#include "AthenaKernel/CLASS_DEF.h"

namespace Tracker {

class FaserSiliconPropertiesVector
{
public:
 
  FaserSiliconPropertiesVector();
  ~FaserSiliconPropertiesVector();

  void resize(IdentifierHash::value_type size);
  const Tracker::FaserSiliconProperties& getSiProperties(const IdentifierHash& elementHash) const;
  void setConditions(const IdentifierHash& elementHash, const double temperature, const double electricField);

private:
  std::vector<Tracker::FaserSiliconProperties> m_data;
  Tracker::FaserSiliconProperties m_defaultProperties;
};

} // namespace Tracker

CLASS_DEF(Tracker::FaserSiliconPropertiesVector, 26711649 , 1)

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF(Tracker::FaserSiliconPropertiesVector, 142567857);

#endif // FASERSIPROPERTIESTOOL_FASERSILICONPROPERTIESVECTOR_H
