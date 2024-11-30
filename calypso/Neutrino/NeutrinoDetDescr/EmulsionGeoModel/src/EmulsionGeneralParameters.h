/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EmulsionGeoModel_EmulsionGeneralParameters_H
#define EmulsionGeoModel_EmulsionGeneralParameters_H

#include "GeoModelKernel/GeoDefinitions.h"

#include <map>
#include <string>

class EmulsionDataBase;
class TopLevelPlacements;

class EmulsionGeneralParameters {

public:

  EmulsionGeneralParameters(EmulsionDataBase* rdb);
  ~EmulsionGeneralParameters();
  //Explicitly disallow copy, assignment to appease coverity
  EmulsionGeneralParameters(const EmulsionGeneralParameters &) = delete;
  EmulsionGeneralParameters & operator=(const EmulsionGeneralParameters &) = delete;

  // General
  double safety() const;

  const GeoTrf::Transform3D & partTransform(const std::string & partName) const; 
  bool partPresent(const std::string & partName) const;

  int nModules() const;
  int nBasesPerModule() const;
  double firstBaseZ() const;
  double lastBaseZ() const;
  
private:

  EmulsionDataBase * m_rdb;
  TopLevelPlacements * m_placements;
};


#endif // EmulsionGeoModel_EmulsionGeneralParameters_H
