/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PRESHOWERGEOMODEL_PRESHOWERDETECTORTOOL_H
#define PRESHOWERGEOMODEL_PRESHOWERDETECTORTOOL_H

#include "GeoModelFaserUtilities/GeoModelTool.h"
#include "PreshowerGeoModel/PreshowerGeoModelAthenaComps.h" 

#include "GeometryDBSvc/IGeometryDBSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

#include "GaudiKernel/ServiceHandle.h"

#include <string>

namespace ScintDD {
  class PreshowerDetectorManager;
}

class PreshowerID;
// class FaserDetectorID;

class PreshowerDetectorTool : public GeoModelTool {

public:
  // Standard Constructor
  PreshowerDetectorTool(const std::string& type, const std::string& name, const IInterface* parent);

  virtual StatusCode create() override final;
  virtual StatusCode clear() override final;

  // Register callback function on ConDB object
  virtual StatusCode registerCallback ATLAS_NOT_THREAD_SAFE () override final;

  // Callback function itself
  virtual StatusCode align(IOVSVC_CALLBACK_ARGS) override;

private:
  StringProperty m_detectorName{this, "DetectorName", "Preshower"};
  BooleanProperty m_alignable{this, "Alignable", true};
  BooleanProperty m_useDynamicAlignFolders{this, "useDynamicAlignFolders", false};
  bool m_cosmic;

  const ScintDD::PreshowerDetectorManager* m_manager;
  
  PreshowerGeoModelAthenaComps m_athenaComps;

  ServiceHandle< IGeoDbTagSvc > m_geoDbTagSvc;
  ServiceHandle< IRDBAccessSvc > m_rdbAccessSvc;
  ServiceHandle< IGeometryDBSvc > m_geometryDBSvc;

  StringProperty m_run1Folder{this,   "Run1Folder",   "/Scint/Align"};
  StringProperty m_run2L1Folder{this, "Run2L1Folder", "/Scint/AlignL1/ID"};
  StringProperty m_run2L2Folder{this, "Run2L2Folder", "/Scint/AlignL2/SCT"};
  StringProperty m_run2L3Folder{this, "Run2L3Folder", "/Scint/AlignL3"};
};

#endif // PRESHOWERGEOMODEL_PRESHOWERDETECTORTOOL_H
