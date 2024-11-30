/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSMATERIALMAPPING_H
#define ACTSMATERIALMAPPING_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkGeometry/MaterialStepCollection.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Material/SurfaceMaterialMapper.hpp"
#include "Acts/Material/VolumeMaterialMapper.hpp"
// PACKAGE
#include "ActsGeometryInterfaces/IActsMaterialTrackWriterSvc.h"
#include "FaserActsGeometryInterfaces/IFaserActsSurfaceMappingTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsVolumeMappingTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsMaterialJsonWriterTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>

namespace Acts {
  class ISurfaceMaterial;
  class IVolumeMaterial;

  using SurfaceMaterialMap
      = std::map<GeometryIdentifier, std::shared_ptr<const ISurfaceMaterial>>;

  using VolumeMaterialMap
      = std::map<GeometryIdentifier, std::shared_ptr<const IVolumeMaterial>>;

  using DetectorMaterialMaps = std::pair<SurfaceMaterialMap, VolumeMaterialMap>;
}


class FaserActsMaterialMapping : public AthReentrantAlgorithm {
public:
  FaserActsMaterialMapping (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

private:
  ServiceHandle<IActsMaterialTrackWriterSvc>      m_materialTrackWriterSvc;
  Gaudi::Property<bool>                           m_mapSurfaces{this, "mapSurfaces", true, "Map the material onto surfaces"};
  Gaudi::Property<bool>                           m_mapVolumes{this, "mapVolumes", true, "Map the material onto volumes"};
  SG::ReadHandleKey<Trk::MaterialStepCollection>  m_inputMaterialStepCollection;
  ToolHandle<IFaserActsSurfaceMappingTool>        m_surfaceMappingTool{this, "SurfaceMappingTool", "FaserActsSurfaceMappingTool"};
  ToolHandle<IFaserActsVolumeMappingTool>         m_volumeMappingTool{this, "VolumeMappingTool", "FaserActsVolumeMappingTool"};
  ToolHandle<IFaserActsMaterialJsonWriterTool>    m_materialJsonWriterTool{this, "MaterialJsonWriterTool", "FaserActsMaterialJsonWriterTool"};

  Acts::MagneticFieldContext                      m_mctx;
  Acts::GeometryContext                           m_gctx;
  Acts::SurfaceMaterialMapper::State              m_mappingState;
  Acts::VolumeMaterialMapper::State               m_mappingStateVol;
};

#endif // ActsGeometry_ActsExtrapolation_h
