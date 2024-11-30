/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GeoModel_SCT_GeometryManager_H
#define FASERSCT_GeoModel_SCT_GeometryManager_H

#include "SCT_Options.h"

#include <memory>

namespace TrackerDD {
  class SiCommonItems;
  class DistortedMaterialManager;
}
class SCT_BarrelParameters;
class SCT_BarrelModuleParameters;
class SCT_DataBase;
class SCT_GeneralParameters;
class SCT_WrappingParameters;
class SCT_GeoModelAthenaComps;

class SCT_GeometryManager {

public:

  // Constructor 
  SCT_GeometryManager(SCT_DataBase* rdb);

  // Destructor 
  ~SCT_GeometryManager();

  // Access to run time options
  const SCT_Options & options() const;
  void setOptions(const SCT_Options & options);

  // Access to athena components
  const SCT_GeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const TrackerDD::SiCommonItems * commonItems() const;

  const SCT_BarrelParameters          * barrelParameters() const;
  const SCT_BarrelModuleParameters    * barrelModuleParameters() const;
  const SCT_GeneralParameters         * generalParameters() const;
  const SCT_WrappingParameters        * wrappingParameters() const;
  const TrackerDD::DistortedMaterialManager  * distortedMatManager() const;

  SCT_GeometryManager& operator=(const SCT_GeometryManager& right);
  SCT_GeometryManager(const SCT_GeometryManager& right);

private:

  SCT_Options m_options;
  const SCT_GeoModelAthenaComps * m_athenaComps;
  TrackerDD::SiCommonItems * m_commonItems;
  SCT_DataBase* m_rdb;
 
  std::unique_ptr<SCT_BarrelParameters> m_barrelParameters;
  std::unique_ptr<SCT_BarrelModuleParameters> m_barrelModuleParameters;
  std::unique_ptr<SCT_GeneralParameters> m_generalParameters;
  std::unique_ptr<SCT_WrappingParameters> m_wrappingParameters;
  std::unique_ptr<TrackerDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // SCT_GeoModel_SCT_GeometryManager_H
