/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSWRITETRACKINGGEOMETRY_H
#define FASERACTSGEOMETRY_ACTSWRITETRACKINGGEOMETRY_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"

// PACKAGE
#include "FaserActsObjWriterTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"

// STL
#include <fstream>
#include <memory>
#include <vector>

namespace Acts {
  class TrackingGeometry;
}

class IFaserActsTrackingGeometrySvc;
class IFaserActsMaterialJsonWriterTool;

class FaserActsWriteTrackingGeometry : public AthReentrantAlgorithm {
public:
  FaserActsWriteTrackingGeometry (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:

  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};

  ToolHandle<FaserActsObjWriterTool> m_objWriterTool{this, "ObjWriterTool", "FaserActsObjWriterTool"};
  ToolHandle<IFaserActsMaterialJsonWriterTool> m_materialJsonWriterTool{this, "MaterialJsonWriterTool", "FaserActsMaterialJsonWriterTool"};

};

#endif
