/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DIPOLEGEOMODEL_DIPOLETOOL_H
#define DIPOLEGEOMODEL_DIPOLETOOL_H

#include "GeoModelFaserUtilities/GeoModelTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

class IGeoDbTagSvc;
class IRDBAccessSvc;
class IGeometryDBSvc;
class DipoleGeoModelAthenaComps;

namespace TrackerDD {
  class DipoleManager;
}

class DipoleTool : public GeoModelTool {

 public: 
  // Standard Constructor
  DipoleTool( const std::string& type, const std::string& name, const IInterface* parent );
  // Standard Destructor
  virtual ~DipoleTool() override final;
  
  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

 private:
  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;

  std::string m_overrideVersionName;
  const TrackerDD::DipoleManager* m_manager;
  DipoleGeoModelAthenaComps * m_athenaComps;
};

#endif // DIPOLEGEOMODEL_DIPOLETOOL_H

