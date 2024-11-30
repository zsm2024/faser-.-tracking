/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TriggerGeoModel_TriggerGeometryManager_H
#define TriggerGeoModel_TriggerGeometryManager_H

#include "TriggerOptions.h"

#include <memory>

namespace ScintDD {
  class ScintCommonItems;
  class DistortedMaterialManager;
}

class TriggerStationParameters;
class TriggerWrappingParameters;
class TriggerPlateParameters;
class TriggerDataBase;
class TriggerGeneralParameters;
class TriggerGeoModelAthenaComps;

class TriggerGeometryManager {

public:

  // Constructor 
  TriggerGeometryManager(TriggerDataBase* rdb);

  // Destructor 
  ~TriggerGeometryManager();

  // Access to run time options
  const TriggerOptions & options() const;
  void setOptions(const TriggerOptions & options);

  // Access to athena components
  const TriggerGeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const ScintDD::ScintCommonItems * commonItems() const;

  const TriggerStationParameters*             stationParameters() const;
  const TriggerWrappingParameters*            wrappingParameters() const;
  const TriggerPlateParameters*               plateParameters() const;
  const TriggerGeneralParameters*             generalParameters() const;
  const ScintDD::DistortedMaterialManager* distortedMatManager() const;

  TriggerGeometryManager& operator=(const TriggerGeometryManager& right);
  TriggerGeometryManager(const TriggerGeometryManager& right);

private:

  TriggerOptions m_options;
  const TriggerGeoModelAthenaComps * m_athenaComps;
  ScintDD::ScintCommonItems * m_commonItems;
  TriggerDataBase* m_rdb;
 
  std::unique_ptr<TriggerStationParameters> m_stationParameters;
  std::unique_ptr<TriggerWrappingParameters> m_wrappingParameters;
  std::unique_ptr<TriggerPlateParameters> m_plateParameters;
  std::unique_ptr<TriggerGeneralParameters> m_generalParameters;
  std::unique_ptr<ScintDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // TriggerGeoModel_TriggerGeometryManager_H
