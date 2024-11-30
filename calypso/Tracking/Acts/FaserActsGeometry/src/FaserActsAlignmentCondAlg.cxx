/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsAlignmentCondAlg.h"

// ATHENA
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "GaudiKernel/ICondSvc.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteCondHandle.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

// PACKAGE
#include "FaserActsGeometry/FaserActsAlignmentStore.h"
#include "FaserActsGeometry/FaserActsDetectorElement.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometrySvc.h"

// ACTS
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Definitions/Common.hpp"

// STL
#include <memory>

FaserActsAlignmentCondAlg::FaserActsAlignmentCondAlg(const std::string &name,
                                           ISvcLocator *pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator), m_cs("CondSvc", name),
      m_trackingGeometrySvc("FaserActsTrackingGeometrySvc", name) {}

FaserActsAlignmentCondAlg::~FaserActsAlignmentCondAlg() {}

StatusCode FaserActsAlignmentCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_sctAlignStoreReadKey.initialize());

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(detStore()->retrieve(p_SCTManager, "SCT"));

  if (m_wchk.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandle with key"
                  << m_wchk.key());
    return StatusCode::FAILURE;
  }

  if (m_cs->regHandle(this, m_wchk).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_wchk.fullKey()
                                                        << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserActsAlignmentCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode FaserActsAlignmentCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  auto trkGeom = m_trackingGeometrySvc->trackingGeometry();

  EventIDBase now(getContext().eventID());
  SG::WriteCondHandle<FaserActsGeometryContext> wch(m_wchk);

  if (wch.isValid(now)) {
    ATH_MSG_DEBUG(
        "CondHandle is already valid for "
        << now << ". In theory this should not be called, but may happen"
        << " if multiple concurrent events are being processed out of order.");

  } else {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() << " not valid now (" << now
                                  << "). Getting new info for dbKey \""
                                  << wch.dbKey() << "\" from CondDb");

    SG::ReadCondHandle<GeoAlignmentStore> sctAlignStore(m_sctAlignStoreReadKey);


    EventIDRange sctRange;
    if(!sctAlignStore.range(sctRange)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << sctAlignStore.key());
      return StatusCode::FAILURE;
    }

    // create an Acts aware geo alignment store from the one given
    // (this makes a copy for now, which is not ideal)
    auto actsAlignStore =
        std::make_unique<FaserActsAlignmentStore>(**sctAlignStore);

    // deltas are set, now populate sensitive element transforms
    ATH_MSG_DEBUG("Populating FaserActsAlignmentStore for IOV");
    size_t nElems = 0;
    trkGeom->visitSurfaces([&actsAlignStore, &nElems](const Acts::Surface *srf) {
      const Acts::DetectorElementBase *detElem =
      srf->associatedDetectorElement();
      if(detElem){
      const auto *gmde = static_cast<const FaserActsDetectorElement *>(detElem);
      gmde->storeTransform(actsAlignStore.get());
      nElems++;
      }
    });
    ATH_MSG_DEBUG("FaserActsAlignmentStore populated for " << nElems
                                                      << " detector elements");

    // get a nominal alignment store from the tracking geometry service
    // and plug it into a geometry context
    auto gctx = std::make_unique<FaserActsGeometryContext>();
    gctx->ownedAlignmentStore =
        std::move(actsAlignStore); // GCTX owns the alignment store
    gctx->alignmentStore = gctx->ownedAlignmentStore.get();

    if (wch.record(sctRange, gctx.release()).isFailure()) {
      ATH_MSG_ERROR("could not record alignment store "
                    << wch.key() << " = " << gctx.get() << " with EventRange "
                    << sctRange);
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Recorded new " << wch.key() << " "
                                       << " with range " << sctRange);
  }



  return StatusCode::SUCCESS;
}
