/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSTRACKINGGEOMETRYTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSTRACKINGGEOMETRYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"

class Identifier;

namespace Acts {
  class TrackingGeometry;
  class GeometryIdentifier;
}


class IFaserActsTrackingGeometryTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IFaserActsTrackingGeometryTool, 1, 0);

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() const = 0;

  virtual
  const FaserActsGeometryContext&
  getGeometryContext(const EventContext& ctx = Gaudi::Hive::currentContext()) const = 0;

  virtual
  FaserActsGeometryContext
  getNominalGeometryContext() const = 0;

  virtual
  const std::shared_ptr<std::map<Identifier, Acts::GeometryIdentifier>>
  getIdentifierMap() const = 0;
};

#endif
