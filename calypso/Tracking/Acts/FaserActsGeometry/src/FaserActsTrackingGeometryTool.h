/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H
#define FASERACTSGEOMETRY_ACTSTRACKINGGEOMETRYTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadCondHandleKey.h"

// PACKAGE
#include "FaserActsGeometry/FaserActsAlignmentStore.h" // ReadCondHandleKey wants complete type
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"

// ACTS

namespace Acts {
class TrackingGeometry;
}

class FaserActsGeometryContext;
class IFaserActsTrackingGeometrySvc;


class FaserActsTrackingGeometryTool : public extends<AthAlgTool, IFaserActsTrackingGeometryTool>
{

public:
  StatusCode initialize() override;

  FaserActsTrackingGeometryTool(const std::string &type, const std::string &name,
                           const IInterface *parent);

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() const override;

  virtual
  const FaserActsGeometryContext&
  getGeometryContext(const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

  virtual
  FaserActsGeometryContext
  getNominalGeometryContext() const override;

  virtual
  const std::shared_ptr<std::map<Identifier, Acts::GeometryIdentifier>>
  getIdentifierMap() const override;

private:
  ServiceHandle<IFaserActsTrackingGeometrySvc> m_trackingGeometrySvc;

  SG::ReadCondHandleKey<FaserActsGeometryContext> m_rchk{
      this, "FaserActsAlignmentKey", "FaserActsAlignment", "cond read key"};
};

#endif
