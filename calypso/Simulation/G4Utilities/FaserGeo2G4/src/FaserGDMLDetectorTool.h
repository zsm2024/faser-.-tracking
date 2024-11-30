/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERGEO2G4_FaserGDMLDetectorTool_H
#define FASERGEO2G4_FaserGDMLDetectorTool_H

// Base classes
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "G4AtlasInterfaces/IDetectorGeometrySvc.h"

#include "G4Transform3D.hh"

// Members

// STL library
#include <string>
#include <vector>

/** @class FaserGDMLDetectorTool
 *
 *  Tool for building detectors out of a GDML description.
 *
 *  @author Andrea Dell'Acqua
 *  @date   2017-02-21
 */

class FaserGDMLDetectorTool final : public DetectorGeometryBase
{
 public:
  // Basic constructor and destructor
  FaserGDMLDetectorTool(const std::string& type, const std::string& name, const IInterface *parent);
  ~FaserGDMLDetectorTool() {}

  /** Athena method. called at initialization time, being customized here */
  virtual StatusCode initialize() override final;

  /** virtual methods being implemented here */

  virtual void BuildGeometry() override final;

 private:

  std::string m_GDMLFileName;
  std::string m_builderName;
  bool m_blGetTopTransform;
  G4Transform3D m_topTransform;
  std::string m_geoDetectorName;
  bool IsTopTransform();
  void SetInitialTransformation();
};

#endif // GEO2G4_FaserGDMLDetectorTool_H
