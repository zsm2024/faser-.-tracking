/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionGeometryManager_H
#define EmulsionGeoModel_EmulsionGeometryManager_H

#include "EmulsionOptions.h"

#include <memory>

namespace NeutrinoDD {
  class NeutrinoCommonItems;
  class DistortedMaterialManager;
}

class EmulsionFilmParameters;
class EmulsionPlatesParameters;
class EmulsionDataBase;
class EmulsionGeneralParameters;
class EmulsionSupportParameters;
class EmulsionGeoModelAthenaComps;

class EmulsionGeometryManager {

public:

  // Constructor 
  EmulsionGeometryManager(EmulsionDataBase* rdb);

  // Destructor 
  ~EmulsionGeometryManager();

  // Access to run time options
  const EmulsionOptions & options() const;
  void setOptions(const EmulsionOptions & options);

  // Access to athena components
  const EmulsionGeoModelAthenaComps * athenaComps() const;

  // To be passed to detector element.
  const NeutrinoDD::NeutrinoCommonItems * commonItems() const;

  const EmulsionFilmParameters*               filmParameters() const;
  const EmulsionPlatesParameters*             platesParameters() const;
  const EmulsionGeneralParameters*            generalParameters() const;
  const EmulsionSupportParameters*            supportParameters() const;
  const NeutrinoDD::DistortedMaterialManager* distortedMatManager() const;

  EmulsionGeometryManager& operator=(const EmulsionGeometryManager& right);
  EmulsionGeometryManager(const EmulsionGeometryManager& right);

private:

  EmulsionOptions m_options;
  const EmulsionGeoModelAthenaComps * m_athenaComps;
  NeutrinoDD::NeutrinoCommonItems * m_commonItems;
  EmulsionDataBase* m_rdb;
 
  std::unique_ptr<EmulsionFilmParameters> m_filmParameters;
  std::unique_ptr<EmulsionPlatesParameters> m_platesParameters;
  std::unique_ptr<EmulsionGeneralParameters> m_generalParameters;
  std::unique_ptr<EmulsionSupportParameters> m_supportParameters;
  std::unique_ptr<NeutrinoDD::DistortedMaterialManager> m_distortedMatManager;

};


#endif // EmulsionGeoModel_EmulsionGeometryManager_H
