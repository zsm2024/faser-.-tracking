/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_GEOMODEL_SCT_DETECTORTOOL_H
#define FASERSCT_GEOMODEL_SCT_DETECTORTOOL_H

#include "GeoModelFaserUtilities/GeoModelTool.h"
#include "FaserSCT_GeoModel/SCT_GeoModelAthenaComps.h" 

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include <string>

namespace TrackerDD {
  class SCT_DetectorManager;
}

class FaserSCT_DetectorTool : public GeoModelTool {

public:
  // Standard Constructor
  FaserSCT_DetectorTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  // Register callback function on ConDB object
  virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;

  // Callback function itself
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

private:
  StringProperty m_detectorName{this, "DetectorName", "SCT"};
  BooleanProperty m_alignable{this, "Alignable", true};
  BooleanProperty m_useDynamicAlignFolders{this, "useDynamicAlignFolders", false};
  bool m_cosmic;
  
  const TrackerDD::SCT_DetectorManager* m_manager;
  
  SCT_GeoModelAthenaComps m_athenaComps;

  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;

  StringProperty m_run1Folder{this, "Run1Folder", "/Tracker/Align"};
  // StringProperty m_run1Folder{this, "Run1Folder", "/Indet/Align"};
  // StringProperty m_run2L1Folder{this, "Run2L1Folder", "/Indet/AlignL1/ID"};
  // StringProperty m_run2L2Folder{this, "Run2L2Folder", "/Indet/AlignL2/SCT"};
  // StringProperty m_run2L3Folder{this, "Run2L3Folder", "/Indet/AlignL3"};
};

#endif // FASERSCT_GEOMODEL_SCT_DETECTORTOOL_H
