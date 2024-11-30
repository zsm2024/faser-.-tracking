/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_AlignCondAlg.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"

#include <memory>

FaserSCT_AlignCondAlg::FaserSCT_AlignCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_writeKey{"SCTAlignmentStore", "SCTAlignmentStore"}
{
  declareProperty("WriteKey", m_writeKey);
}

StatusCode FaserSCT_AlignCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Read Handles
  // Static
  ATH_CHECK(m_readKeyStatic.initialize(!m_useDynamicAlignFolders.value()));
  // Dynamic
//   ATH_CHECK(m_readKeyDynamicL1.initialize(m_useDynamicAlignFolders.value()));
//   ATH_CHECK(m_readKeyDynamicL2.initialize(m_useDynamicAlignFolders.value()));
//   ATH_CHECK(m_readKeyDynamicL3.initialize(m_useDynamicAlignFolders.value()));

  // Write Handles
  ATH_CHECK(m_writeKey.initialize());

  // Register write handle
  ATH_CHECK(m_condSvc->regHandle(this, m_writeKey));

  ATH_CHECK(detStore()->retrieve(m_detManager, "SCT"));

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_AlignCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // ____________ Construct Write Cond Handle and check its validity ____________
  SG::WriteCondHandle<GeoAlignmentStore> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  // Get SiDetectorElements
  const TrackerDD::SiDetectorElementCollection* oldColl{m_detManager->getDetectorElementCollection()};
  if (oldColl==nullptr) {
    ATH_MSG_FATAL("Null pointer is returned by getDetectorElementCollection()");
    return StatusCode::FAILURE;
  }

  // ____________ Construct new Write Cond Object ____________
  std::unique_ptr<GeoAlignmentStore> writeCdo{std::make_unique<GeoAlignmentStore>()};
  EventIDRange rangeW;

  if (not m_useDynamicAlignFolders.value()) { // Static
    // ____________ Get Read Cond Object ____________
    SG::ReadCondHandle<AlignableTransformContainer> readHandleStatic{m_readKeyStatic};
    const AlignableTransformContainer* readCdoStatic{*readHandleStatic};
    if (readCdoStatic==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_readKeyStatic.key());
      return StatusCode::FAILURE;
    }
    // ____________ Apply alignments to SCT GeoModel ____________
    // Construct Container for read CDO.
    TrackerDD::RawAlignmentObjects readCdoContainerStatic;
    readCdoContainerStatic.emplace(m_readKeyStatic.key(), readCdoStatic);
    ATH_CHECK(m_detManager->align(readCdoContainerStatic, writeCdo.get()));

    // Define validity of the output cond object and record it
    if (not readHandleStatic.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleStatic.key());
      return StatusCode::FAILURE;
    }
  } else { // Dynamic
    ATH_MSG_FATAL("Dynamic alignment not supported");
    // // ____________ Get Read Cond Object ____________
    // SG::ReadCondHandle<CondAttrListCollection> readHandleDynamicL1{m_readKeyDynamicL1};
    // const CondAttrListCollection* readCdoDynamicL1{*readHandleDynamicL1};
    // if (readCdoDynamicL1==nullptr) {
    //   ATH_MSG_FATAL("Null pointer to the read conditions object of " << m_readKeyDynamicL1.key());
    //   return StatusCode::FAILURE;
    // }
    // SG::ReadCondHandle<CondAttrListCollection> readHandleDynamicL2{m_readKeyDynamicL2};
    // const CondAttrListCollection* readCdoDynamicL2{*readHandleDynamicL2};
    // if (readCdoDynamicL2==nullptr) {
    //   ATH_MSG_FATAL("Null pointer to the read conditions object of " << readHandleDynamicL2.key());
    //   return StatusCode::FAILURE;
    // }
    // SG::ReadCondHandle<AlignableTransformContainer> readHandleDynamicL3{m_readKeyDynamicL3};
    // const AlignableTransformContainer* readCdoDynamicL3{*readHandleDynamicL3};
    // if (readCdoDynamicL3==nullptr) {
    //   ATH_MSG_FATAL("Null pointer to the read conditions object of " << readHandleDynamicL3.key());
    //   return StatusCode::FAILURE;
    // }
    // // ____________ Apply alignments to SCT GeoModel ____________
    // // Construct Container for read CDO-s.
    // TrackerDD::RawAlignmentObjects readCdoContainerDynamicL1;
    // readCdoContainerDynamicL1.emplace(m_readKeyDynamicL1.key(), readCdoDynamicL1);
    // ATH_CHECK(m_detManager->align(readCdoContainerDynamicL1, writeCdo.get()));
    // TrackerDD::RawAlignmentObjects readCdoContainerDynamicL2;
    // readCdoContainerDynamicL2.emplace(m_readKeyDynamicL2.key(), readCdoDynamicL2);
    // ATH_CHECK(m_detManager->align(readCdoContainerDynamicL2, writeCdo.get()));
    // TrackerDD::RawAlignmentObjects readCdoContainerDynamicL3;
    // readCdoContainerDynamicL3.emplace(m_readKeyDynamicL3.key(), readCdoDynamicL3);
    // ATH_CHECK(m_detManager->align(readCdoContainerDynamicL3, writeCdo.get()));

    // // Define validity of the output cond object and record it
    // EventIDRange rangeWL1;
    // if (not readHandleDynamicL1.range(rangeWL1)) {
    //   ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleDynamicL1.key());
    //   return StatusCode::FAILURE;
    // }
    // EventIDRange rangeWL2;
    // if (not readHandleDynamicL2.range(rangeWL2)) {
    //   ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleDynamicL2.key());
    //   return StatusCode::FAILURE;
    // }
    // EventIDRange rangeWL3;
    // if (not readHandleDynamicL3.range(rangeWL3)) {
    //   ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandleDynamicL3.key());
    //   return StatusCode::FAILURE;
    // }
    // rangeW = EventIDRange::intersect(rangeWL1, rangeWL2, rangeWL3);
  }

  // Set (default) absolute transforms in alignment store by calling them.
  for (const TrackerDD::SiDetectorElement* oldEl: *oldColl) {
    oldEl->getMaterialGeom()->getAbsoluteTransform(writeCdo.get());
    oldEl->getMaterialGeom()->getDefAbsoluteTransform(writeCdo.get());
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record GeoAlignmentStore " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_AlignCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
