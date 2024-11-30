/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_FASERACTSSURFACEMAPPINGTOOL_H
#define FASERACTSGEOMETRY_FASERACTSSURFACEMAPPINGTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"

// PACKAGE
#include "FaserActsGeometryInterfaces/IFaserActsSurfaceMappingTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"


// ACTS
#include "Acts/Material/SurfaceMaterialMapper.hpp"

// BOOST
#include <cmath>

class FaserActsSurfaceMappingTool : public extends<AthAlgTool, IFaserActsSurfaceMappingTool>
{

public:
  virtual StatusCode initialize() override;

  FaserActsSurfaceMappingTool(const std::string& type, const std::string& name,
	           const IInterface* parent);

  virtual
  std::shared_ptr<Acts::SurfaceMaterialMapper>
  mapper() const override
  {
    return m_mapper;
  };

  virtual
  Acts::SurfaceMaterialMapper::State
  mappingState() const override;

  virtual
  const IFaserActsTrackingGeometryTool*
  trackingGeometryTool() const override
  {
    return m_trackingGeometryTool.get();
  }


private:
  // Straight line stepper
  Acts::MagneticFieldContext m_magFieldContext;
  Acts::GeometryContext      m_geoContext;
  using SlStepper  = Acts::StraightLineStepper;
  using StraightLinePropagator = Acts::Propagator<SlStepper, Acts::Navigator>;
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool{this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
  std::shared_ptr<Acts::SurfaceMaterialMapper> m_mapper;
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;
};

#endif