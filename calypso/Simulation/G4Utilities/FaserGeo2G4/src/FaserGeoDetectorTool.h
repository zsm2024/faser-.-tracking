/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGeoDetectorTool_H
#define FASERGEO2G4_FaserGeoDetectorTool_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
//#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"
#include "G4FaserInterfaces/IFaserGeo2G4Svc.h"

#include "G4Transform3D.hh"

// Members

// STL library
#include <string>
#include <vector>

/** @class GeoDetectorTool GeoDetectorTool.h "G4AtlasTools/GeoDetectorTool.h"
*
*  Tool for building detectors out of a GeoModel description. Basically a
*  carbon copy of GeoDetectorFacility in GeoDetectorPlugins which is supposed
*  to replace.
*
*  @author Andrea Dell'Acqua
*  @date   2015-03-10
*/

class FaserGeoDetectorTool final : public DetectorGeometryBase
{
public:
  // Basic constructor and destructor
  FaserGeoDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~FaserGeoDetectorTool() {}

  /** Athena method. called at initialization time, being customized here */
  virtual StatusCode initialize() override final;

  /** virtual methods being implemented here */

  virtual void BuildGeometry() override final;

  virtual void PositionInParent() override final;

private:

  std::string m_builderName;
  std::string m_dumpGDMLFile="";
  bool m_blParamOn;
  bool m_blGetTopTransform;
  G4Transform3D m_topTransform;
  std::string m_geoDetectorName;
  G4LogicalVolume* Convert();
  bool IsTopTransform();
  void SetInitialTransformation();
  ServiceHandle<IFaserGeo2G4Svc> m_geo2G4Svc{this, "FaserGeo2G4Svc", "FaserGeo2G4Svc", ""};
};

#endif

