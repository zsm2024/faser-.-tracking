/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoGeoModel_VetoGeometryManager_H
#define VetoGeoModel_VetoGeometryManager_H

#include "VetoOptions.h"

#include <memory>

namespace ScintDD {
  class ScintCommonItems;
  class DistortedMaterialManager;
}

class VetoStationParameters;
class VetoPlateParameters;
class VetoRadiatorParameters;
class VetoWrappingParameters;
class VetoDataBase;
class VetoGeneralParameters;
class VetoGeoModelAthenaComps;

class VetoGeometryManager {

public:

  // Constructor 
  VetoGeometryManager(VetoDataBase* rdb);

  // Destructor 
  ~VetoGeometryManager();

  // Access to run time options
  const VetoOptions & options() const;
  void setOptions(const VetoOptions & options);

  // Access to athena components
  const VetoGeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const ScintDD::ScintCommonItems * commonItems() const;

  const VetoStationParameters*             stationParameters() const;
  const VetoPlateParameters*               plateParameters() const;
  const VetoRadiatorParameters*            radiatorParameters() const;
  const VetoWrappingParameters*            wrappingParameters() const;
  const VetoGeneralParameters*             generalParameters() const;
  const ScintDD::DistortedMaterialManager* distortedMatManager() const;

  VetoGeometryManager& operator=(const VetoGeometryManager& right);
  VetoGeometryManager(const VetoGeometryManager& right);

private:

  VetoOptions m_options;
  const VetoGeoModelAthenaComps * m_athenaComps;
  ScintDD::ScintCommonItems * m_commonItems;
  VetoDataBase* m_rdb;
 
  std::unique_ptr<VetoStationParameters> m_stationParameters;
  std::unique_ptr<VetoPlateParameters> m_plateParameters;
  std::unique_ptr<VetoRadiatorParameters> m_radiatorParameters;
  std::unique_ptr<VetoWrappingParameters> m_wrappingParameters;
  std::unique_ptr<VetoGeneralParameters> m_generalParameters;
  std::unique_ptr<ScintDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // VetoGeoModel_VetoGeometryManager_H
