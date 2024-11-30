/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSTRACKINGGEOMETRYSVC_H
#define FASERACTSGEOMETRY_ACTSTRACKINGGEOMETRYSVC_H

// ATHENA
#include "AthenaBaseComps/AthService.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/EventContext.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"

// ACTS
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Units.hpp"

// PACKAGE
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometrySvc.h"

// STL
#include <map>

using namespace Acts::UnitLiterals;

namespace TrackerDD {
  class SCT_DetectorManager;
  class DipoleManager;
}

class FaserActsAlignmentStore;

class FaserActsDetectorElement;

namespace Acts {

class TrackingGeometry;
class CylinderVolumeHelper;
class CylinderVolumeBuilder;
class ITrackingVolumeBuilder;

class GeometryID;
class BinnedSurfaceMaterial;

}


class FaserActsTrackingGeometrySvc : public extends<AthService, IFaserActsTrackingGeometrySvc> {
public:

  StatusCode initialize() override;
  //virtual StatusCode finalize() override;

  FaserActsTrackingGeometrySvc( const std::string& name, ISvcLocator* pSvcLocator );

  std::shared_ptr<const Acts::TrackingGeometry>
  trackingGeometry() override;

  void
  populateAlignmentStore(FaserActsAlignmentStore *store) const override;

  void
  populateIdentifierMap(IdentifierMap *map) const override;

  const FaserActsAlignmentStore*
  getNominalAlignmentStore() const override;

  std::shared_ptr<IdentifierMap>
  getIdentifierMap() const override;

private:
  std::shared_ptr<const Acts::ITrackingVolumeBuilder>
  makeVolumeBuilder(const Acts::GeometryContext& gctx, const TrackerDD::SCT_DetectorManager* manager, const GeoVDetectorManager* vetoManager, const GeoVDetectorManager* triggerManager, const GeoVDetectorManager* dipoleManager);

  ServiceHandle<StoreGateSvc> m_detStore;
  const TrackerDD::SCT_DetectorManager* p_SCTManager;

  std::shared_ptr<std::vector<std::shared_ptr<const FaserActsDetectorElement>>> m_elementStore;
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;
  std::shared_ptr<std::map<Identifier, Acts::GeometryIdentifier>> m_identifierMap;

  std::unique_ptr<const FaserActsAlignmentStore> m_nominalAlignmentStore{nullptr};

  Gaudi::Property<bool> m_useMaterialMap{this, "UseMaterialMap", false, ""};
  Gaudi::Property<std::string> m_materialMapInputFile{this, "MaterialMapInputFile", "", ""};
  Gaudi::Property<std::vector<size_t>> m_MaterialBins{this, "MaterialBins", {30, 30}};
};



#endif
