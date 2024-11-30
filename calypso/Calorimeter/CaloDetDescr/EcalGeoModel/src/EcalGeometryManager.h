/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EcalGeoModel_EcalGeometryManager_H
#define EcalGeoModel_EcalGeometryManager_H

#include "EcalOptions.h"

#include <memory>

namespace CaloDD {
  class CaloCommonItems;
  class DistortedMaterialManager;
}

class EcalRowParameters;
// class EcalModuleParameters;
class EcalDataBase;
class EcalGeneralParameters;
class EcalGeoModelAthenaComps;

class EcalGeometryManager {

public:

  // Constructor 
  EcalGeometryManager(EcalDataBase* rdb);

  // Destructor 
  ~EcalGeometryManager();

  // Access to run time options
  const EcalOptions & options() const;
  void setOptions(const EcalOptions & options);

  // Access to athena components
  const EcalGeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const CaloDD::CaloCommonItems * commonItems() const;

  const EcalRowParameters*             rowParameters() const;
  // const EcalModuleParameters*               moduleParameters() const;
  const EcalGeneralParameters*             generalParameters() const;
  const CaloDD::DistortedMaterialManager* distortedMatManager() const;

  EcalGeometryManager& operator=(const EcalGeometryManager& right);
  EcalGeometryManager(const EcalGeometryManager& right);

private:

  EcalOptions m_options;
  const EcalGeoModelAthenaComps * m_athenaComps;
  CaloDD::CaloCommonItems * m_commonItems;
  EcalDataBase* m_rdb;
 
  std::unique_ptr<EcalRowParameters> m_rowParameters;
  // std::unique_ptr<EcalModuleParameters> m_moduleParameters;
  std::unique_ptr<EcalGeneralParameters> m_generalParameters;
  std::unique_ptr<CaloDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // EcalGeoModel_EcalGeometryManager_H
