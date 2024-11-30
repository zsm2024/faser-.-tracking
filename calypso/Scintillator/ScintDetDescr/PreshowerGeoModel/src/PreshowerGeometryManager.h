/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PreshowerGeoModel_PreshowerGeometryManager_H
#define PreshowerGeoModel_PreshowerGeometryManager_H

#include "PreshowerOptions.h"

#include <memory>

namespace ScintDD {
  class ScintCommonItems;
  class DistortedMaterialManager;
}

class PreshowerStationParameters;
class PreshowerPlateParameters;
class PreshowerRadiatorParameters;
class PreshowerAbsorberParameters;
class PreshowerWrappingParameters;
class PreshowerDataBase;
class PreshowerGeneralParameters;
class PreshowerGeoModelAthenaComps;

class PreshowerGeometryManager {

public:

  // Constructor 
  PreshowerGeometryManager(PreshowerDataBase* rdb);

  // Destructor 
  ~PreshowerGeometryManager();

  // Access to run time options
  const PreshowerOptions & options() const;
  void setOptions(const PreshowerOptions & options);

  // Access to athena components
  const PreshowerGeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const ScintDD::ScintCommonItems * commonItems() const;

  const PreshowerStationParameters*             stationParameters() const;
  const PreshowerPlateParameters*               plateParameters() const;
  const PreshowerRadiatorParameters*            radiatorParameters() const;
  const PreshowerAbsorberParameters*            absorberParameters() const;
  const PreshowerGeneralParameters*             generalParameters() const;
  const PreshowerWrappingParameters*            wrappingParameters() const;
  const ScintDD::DistortedMaterialManager* distortedMatManager() const;

  PreshowerGeometryManager& operator=(const PreshowerGeometryManager& right);
  PreshowerGeometryManager(const PreshowerGeometryManager& right);

private:

  PreshowerOptions m_options;
  const PreshowerGeoModelAthenaComps * m_athenaComps;
  ScintDD::ScintCommonItems * m_commonItems;
  PreshowerDataBase* m_rdb;
 
  std::unique_ptr<PreshowerStationParameters> m_stationParameters;
  std::unique_ptr<PreshowerPlateParameters> m_plateParameters;
  std::unique_ptr<PreshowerRadiatorParameters> m_radiatorParameters;
  std::unique_ptr<PreshowerAbsorberParameters> m_absorberParameters;
  std::unique_ptr<PreshowerGeneralParameters> m_generalParameters;
  std::unique_ptr<PreshowerWrappingParameters> m_wrappingParameters;
  std::unique_ptr<ScintDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // PreshowerGeoModel_PreshowerGeometryManager_H
