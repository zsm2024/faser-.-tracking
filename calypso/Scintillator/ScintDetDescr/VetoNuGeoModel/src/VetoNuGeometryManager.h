/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VetoNuGeoModel_VetoNuGeometryManager_H
#define VetoNuGeoModel_VetoNuGeometryManager_H

#include "VetoNuOptions.h"

#include <memory>

namespace ScintDD {
  class ScintCommonItems;
  class DistortedMaterialManager;
}

class VetoNuStationParameters;
class VetoNuPlateParameters;
class VetoNuRadiatorParameters;
class VetoNuDataBase;
class VetoNuGeneralParameters;
class VetoNuGeoModelAthenaComps;

class VetoNuGeometryManager {

public:

  // Constructor 
  VetoNuGeometryManager(VetoNuDataBase* rdb);

  // Destructor 
  ~VetoNuGeometryManager();

  // Access to run time options
  const VetoNuOptions & options() const;
  void setOptions(const VetoNuOptions & options);

  // Access to athena components
  const VetoNuGeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const ScintDD::ScintCommonItems * commonItems() const;

  const VetoNuStationParameters*             stationParameters() const;
  const VetoNuPlateParameters*               plateParameters() const;
  const VetoNuRadiatorParameters*            radiatorParameters() const;
  const VetoNuGeneralParameters*             generalParameters() const;
  const ScintDD::DistortedMaterialManager* distortedMatManager() const;

  VetoNuGeometryManager& operator=(const VetoNuGeometryManager& right);
  VetoNuGeometryManager(const VetoNuGeometryManager& right);

private:

  VetoNuOptions m_options;
  const VetoNuGeoModelAthenaComps * m_athenaComps;
  ScintDD::ScintCommonItems * m_commonItems;
  VetoNuDataBase* m_rdb;
 
  std::unique_ptr<VetoNuStationParameters> m_stationParameters;
  std::unique_ptr<VetoNuPlateParameters> m_plateParameters;
  std::unique_ptr<VetoNuRadiatorParameters> m_radiatorParameters;
  std::unique_ptr<VetoNuGeneralParameters> m_generalParameters;
  std::unique_ptr<ScintDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // VetoNuGeoModel_VetoNuGeometryManager_H
