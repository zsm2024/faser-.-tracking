#pragma once

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "TrkTrack/TrackCollection.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"

class TTree;
class FaserSCT_ID;

class OverlayRDOAlg : public AthReentrantAlgorithm {
public:
  OverlayRDOAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~OverlayRDOAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext &ctx) const override;
  virtual StatusCode finalize() override;

private:

  void processContainer(const FaserSCT_RDO_Container& container, std::map<IdentifierHash, std::vector<const FaserSCT_RDO_Collection*> >& rdoDB) const;
  void processDB(const std::map<IdentifierHash, std::vector<const FaserSCT_RDO_Collection*> >& db, SG::WriteHandle<FaserSCT_RDO_Container>& outContainer, std::function<bool(int)> lambda) const;
  Trk::Track* cloneTrack(const Trk::Track* originalTrack) const;

  SG::ReadHandleKey<TrackCollection> m_posTrackKey { this, "PosTrackCollection", "Pos_CKFTrackCollectionWithoutIFT", "Input track collection name" };
  SG::ReadHandleKey<TrackCollection> m_negTrackKey { this, "NegTrackCollection", "Neg_CKFTrackCollectionWithoutIFT", "Input track collection name" };
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_posRdoKey { this, "PosRdoContainer", "Pos_SCT_RDOs"};
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_negRdoKey { this, "NegRdoContainer", "Neg_SCT_RDOs"};
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_posEdgeModeRdoKey { this, "PosEdgeModeRdoContainer", "Pos_SCT_EDGEMODE_RDOs"};
  SG::ReadHandleKey<FaserSCT_RDO_Container> m_negEdgeModeRdoKey { this, "NegEdgeModeRdoContainer", "Neg_SCT_EDGEMODE_RDOs"};

  SG::WriteHandleKey<FaserSCT_RDO_Container> m_outRdoKey{this, "OutputRDOObjectName", "SCT_RDOs", "Output RDO Object name"};
  SG::WriteHandleKey<FaserSCT_RDO_Container> m_outEdgeModeRdoKey{this, "OutputEDGEMODEObjectName", "SCT_EDGEMODE_RDOs", "Output EDGEMODE Object name"};
  SG::WriteHandleKey<TrackCollection> m_outputTrackCollection { this, "OutputTrackCollection", "Orig_CKFTrackCollectionWithoutIFT", "Output track collection name"};

  const FaserSCT_ID* m_sctHelper;

};

