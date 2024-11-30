/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_DetectorElementCondAlg.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrkGeometry/Layer.h"
#include "TrkSurfaces/Surface.h"

#include <map>

FaserSCT_DetectorElementCondAlg::FaserSCT_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthReentrantAlgorithm(name, pSvcLocator)
  , m_readKey{"SCTAlignmentStore", "SCTAlignmentStore"}
{
  declareProperty("ReadKey", m_readKey);
}

StatusCode FaserSCT_DetectorElementCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // Read Handle
  ATH_CHECK(m_readKey.initialize());

  // Write Handle
  ATH_CHECK(m_writeKey.initialize());
  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());
  // Register write handle
  ATH_CHECK(m_condSvc->regHandle(this, m_writeKey));

  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_DetectorElementCondAlg::execute(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<TrackerDD::SiDetectorElementCollection> writeHandle{m_writeKey, ctx};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  const TrackerDD::SiDetectorElementCollection* oldColl{m_detManager->getDetectorElementCollection()};
  if (oldColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  // ____________ Construct new Write Cond Object ____________
  std::unique_ptr<TrackerDD::SiDetectorElementCollection> writeCdo{std::make_unique<TrackerDD::SiDetectorElementCollection>()};
  EventIDRange rangeW;

  // ____________ Get Read Cond Object ____________
  SG::ReadCondHandle<GeoAlignmentStore> readHandle{m_readKey, ctx};
  const GeoAlignmentStore* readCdo{*readHandle};
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_readKey.key());
    return StatusCode::FAILURE;
  }

  // Define validity of the output cond object and record it
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  // ____________ Update writeCdo using readCdo ____________
  std::map<const TrackerDD::SiDetectorElement*, const TrackerDD::SiDetectorElement*> oldToNewMap;
  oldToNewMap[nullptr] = nullptr;
  writeCdo->resize(oldColl->size(), nullptr);
  TrackerDD::SiDetectorElementCollection::iterator newEl{writeCdo->begin()};
  for (const TrackerDD::SiDetectorElement* oldEl: *oldColl) {
    *newEl = new TrackerDD::SiDetectorElement(oldEl->identify(),
                                            &(oldEl->design()),
                                            oldEl->GeoVDetectorElement::getMaterialGeom(),
                                            oldEl->getCommonItems(),
                                            readCdo);
    oldToNewMap[oldEl] = *newEl;
    newEl++;
  }

  // Set neighbours and other side
  // Set layer to surface
  TrackerDD::SiDetectorElementCollection::const_iterator oldIt{oldColl->begin()};
  for (TrackerDD::SiDetectorElement* newEl: *writeCdo) {
    if (oldToNewMap[(*oldIt)]!=newEl) {
      ATH_MSG_ERROR("Old and new elements are not synchronized!");
    }
    newEl->setNextInEta(oldToNewMap[(*oldIt)->nextInEta()]);
    newEl->setPrevInEta(oldToNewMap[(*oldIt)->prevInEta()]);
    newEl->setNextInPhi(oldToNewMap[(*oldIt)->nextInPhi()]);
    newEl->setPrevInPhi(oldToNewMap[(*oldIt)->prevInPhi()]);
    newEl->setOtherSide(oldToNewMap[(*oldIt)->otherSide()]);
    // Layer of old element is set by InDet::SiLayerBuilder::registerSurfacesToLayer.
    const Trk::Layer* layer{(*oldIt)->surface().associatedLayer()};
    if (layer) {
      newEl->surface().associateLayer(*layer);
    }
    oldIt++;
  }

  // Apply alignment using readCdo passed to SiDetectorElement
  for (TrackerDD::SiDetectorElement* newEl: *writeCdo) {
    newEl->setCache();
  }

  // Record WriteCondHandle
  const std::size_t size{writeCdo->size()};
  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " with size of " << size << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_DetectorElementCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());

  return StatusCode::SUCCESS;
}
