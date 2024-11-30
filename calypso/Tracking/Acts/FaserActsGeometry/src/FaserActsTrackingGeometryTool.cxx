/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsTrackingGeometryTool.h"

// ATHENA
#include "GaudiKernel/EventContext.h"

// PACKAGE
#include "FaserActsGeometry/FaserActsAlignmentStore.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometrySvc.h"

// STL
#include <iostream>
#include <memory>

FaserActsTrackingGeometryTool::FaserActsTrackingGeometryTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : base_class(type, name, parent),
    m_trackingGeometrySvc("FaserActsTrackingGeometrySvc", name)
{
}

StatusCode
FaserActsTrackingGeometryTool::initialize()
{
  ATH_MSG_INFO(name() << " initializing");

  ATH_CHECK( m_trackingGeometrySvc.retrieve() );

  ATH_CHECK( m_rchk.initialize() );

  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
FaserActsTrackingGeometryTool::trackingGeometry() const
{
  return m_trackingGeometrySvc->trackingGeometry();
}

const FaserActsGeometryContext&
FaserActsTrackingGeometryTool::getGeometryContext(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Creating alignment context for event");
  SG::ReadCondHandle<FaserActsGeometryContext> rch(m_rchk, ctx);

  if(!rch.isValid()) {
    ATH_MSG_ERROR("Creating alignment context failed: read cond handle invalid!");
  }

  return **rch;
}

FaserActsGeometryContext
FaserActsTrackingGeometryTool::getNominalGeometryContext() const
{

  FaserActsGeometryContext gctx;
  gctx.alignmentStore = m_trackingGeometrySvc->getNominalAlignmentStore();

  return gctx;
}

const std::shared_ptr<std::map<Identifier, Acts::GeometryIdentifier>>
FaserActsTrackingGeometryTool::getIdentifierMap() const
{
  return m_trackingGeometrySvc->getIdentifierMap();
}
