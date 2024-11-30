/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRYINTERFACES_IACTSTRACKINGGEOMETRYSVC_H
#define FASERACTSGEOMETRYINTERFACES_IACTSTRACKINGGEOMETRYSVC_H

#include "GaudiKernel/IInterface.h"

class EventContext;
class FaserActsAlignmentStore;
class Identifier;

namespace Acts {
  class TrackingGeometry;
  class GeometryIdentifier;
}


using IdentifierMap = std::map<Identifier, Acts::GeometryIdentifier>;

class IFaserActsTrackingGeometrySvc : virtual public IInterface {
  public:

  DeclareInterfaceID(IFaserActsTrackingGeometrySvc, 1, 0);

  virtual
  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() = 0;

  virtual
  void
  populateAlignmentStore(FaserActsAlignmentStore *store) const = 0;

  virtual
  const FaserActsAlignmentStore*
  getNominalAlignmentStore() const = 0;

  virtual
  void
  populateIdentifierMap(IdentifierMap *map) const = 0;

  virtual
  std::shared_ptr<IdentifierMap>
  getIdentifierMap() const = 0;
};

#endif
