/*
  Copyright (C) 2020 CERN for the benefit of the FASER collaboration
*/

/*
 * TrackerByteStreamAccess.h
 * 
 * Simple algorithm to access TLB data from storegate.
 * Try to write a proper thread-safe algorithm.
 *
 */

#ifndef TRACKERBYTESTREAM_TRACKERDATAACCESS_H
#define TRACKERBYTESTREAM_TRACKERDATAACCESS_H 

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include <atomic>

#include "TrackerRawData/FaserSCT_RDO_Container.h"

class TrackerDataAccess: public AthReentrantAlgorithm
{
 public:
  TrackerDataAccess(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TrackerDataAccess();

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;

 private:

  StatusCode checkContainer(const EventContext& ctx,
                            const SG::ReadHandleKey<FaserSCT_RDO_Container>& handleKey, 
                            std::atomic<size_t>& counter) const;


  mutable std::atomic<size_t> m_events{0};
  mutable std::atomic<size_t> m_goodEvents{0};
  mutable std::atomic<size_t> m_hitModeHits{0};
  mutable std::atomic<size_t> m_levelModeHits{0};
  mutable std::atomic<size_t> m_edgeModeHits{0};

  /// StoreGate keys (different hit mode emulation)
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_TrackerContainer_HITMODE
    { this, "TrackerHitModeContainerKey", "SCT_RDOs", "ReadHandleKey for SCT_RDOs Container"};
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_TrackerContainer_LEVELMODE
    { this, "TrackerLevelModeContainerKey", "SCT_LEVELMODE_RDOs", "ReadHandleKey for SCT_LEVELMODE_RDOs Container"};
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_TrackerContainer_EDGEMODE
    { this, "TrackerEdgeModeContainerKey", "SCT_EDGEMODE_RDOs", "ReadHandleKey for SCT_EDGEMODE_RDOs Container"};



};
#endif /* TRACKERBYTESTREAM_TRACKERDATAACCESS_H */
