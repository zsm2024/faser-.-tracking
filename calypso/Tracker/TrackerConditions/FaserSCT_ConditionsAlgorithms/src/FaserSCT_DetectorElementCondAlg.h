// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
#define FASERSCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelFaserUtilities/GeoAlignmentStore.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"

namespace TrackerDD {
  class SCT_DetectorManager;
}

class FaserSCT_DetectorElementCondAlg : public AthReentrantAlgorithm
{
 public:
  FaserSCT_DetectorElementCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_DetectorElementCondAlg() override = default;

  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<GeoAlignmentStore> m_readKey;
  SG::WriteCondHandleKey<TrackerDD::SiDetectorElementCollection> m_writeKey{this, "WriteKey", "SCT_DetectorElementCollection", "Key of output SiDetectorElementCollection for SCT"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
  const TrackerDD::SCT_DetectorManager* m_detManager{nullptr};
};

#endif // FASERSCT_CONDITIONSALGORITHMS_SCT_DETECTORELEMENTCONDALG_H
