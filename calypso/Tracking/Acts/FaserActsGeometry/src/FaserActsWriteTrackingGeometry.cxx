/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "FaserActsWriteTrackingGeometry.h"

// ATHENA
#include "AthenaKernel/RNGWrapper.h"
#include "Acts/Utilities/Logger.hpp"
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"

// PACKAGE
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometrySvc.h"
#include "FaserActsGeometryInterfaces/IFaserActsMaterialJsonWriterTool.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "ActsInterop/Logger.h"

// STL
#include <string>

FaserActsWriteTrackingGeometry::FaserActsWriteTrackingGeometry(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode FaserActsWriteTrackingGeometry::initialize() {
  ATH_MSG_INFO("initializing");

  ATH_CHECK(m_objWriterTool.retrieve());
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_materialJsonWriterTool.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode FaserActsWriteTrackingGeometry::execute(const EventContext& /*ctx*/ ) const {
  auto trackingGeometry = m_trackingGeometryTool->trackingGeometry();

  // Use nominal context
  //FaserActsGeometryContext defGctx;
  //const FaserActsGeometryContext& gctx = m_trackingGeometryTool->getGeometryContext(ctx);
  const FaserActsGeometryContext& gctx = m_trackingGeometryTool->getNominalGeometryContext();

  ATH_MSG_INFO("start to write object");
  m_objWriterTool->write(gctx, *trackingGeometry);
  ATH_MSG_INFO("start to write geometry");
  m_materialJsonWriterTool->write(*trackingGeometry);
  return StatusCode::SUCCESS;
}

StatusCode FaserActsWriteTrackingGeometry::finalize() {
  return StatusCode::SUCCESS;
}
