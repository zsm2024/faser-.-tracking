/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsSurfaceMappingTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
// PACKAGE
#include "ActsInterop/Logger.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
// #include "FaserActsTrackingGeometryTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"

// ACTS
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

// STL
#include <iostream>
#include <memory>


FaserActsSurfaceMappingTool::FaserActsSurfaceMappingTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
{
}

StatusCode
FaserActsSurfaceMappingTool::initialize()
{
  ATH_MSG_INFO("Initializing ACTS Surface Mapper");

  ATH_CHECK( m_trackingGeometryTool.retrieve() );

  m_trackingGeometry = m_trackingGeometryTool->trackingGeometry();

  Acts::Navigator navigator( Acts::Navigator::Config{ m_trackingGeometry } );
  // Make stepper and propagator
  SlStepper stepper;
  StraightLinePropagator propagator = StraightLinePropagator(std::move(stepper), std::move(navigator));

  /// The material mapper
  Acts::SurfaceMaterialMapper::Config smmConfig;
  smmConfig.mapperDebugOutput = true;
  m_mapper = std::make_shared<Acts::SurfaceMaterialMapper>(
      smmConfig,
      std::move(propagator),
      makeActsAthenaLogger(this, "SurfaceMaterialMapper"));

  m_geoContext = m_trackingGeometryTool->getNominalGeometryContext().context();

  ATH_MSG_INFO("ACTS Surface Mapper successfully initialized");
  return StatusCode::SUCCESS;
}

Acts::SurfaceMaterialMapper::State
FaserActsSurfaceMappingTool::mappingState() const
{
  auto mappingState = m_mapper->createState(
    m_geoContext, m_magFieldContext, *m_trackingGeometry);

  return mappingState;
}
