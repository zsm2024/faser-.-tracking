/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSALIGNMENTCONDALG_H
#define FASERACTSGEOMETRY_ACTSALIGNMENTCONDALG_H

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventInfo/EventInfo.h" // ReadHandleKey wants complete type
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/WriteCondHandleKey.h"

// PACKAGE

// STL
#include <string>

namespace TrackerDD {
class SCT_DetectorManager;
} 

class EventInfo;
class ICondSvc;
class StoreGateSvc;
class IFaserActsTrackingGeometrySvc;
class FaserActsAlignmentStore;
class GeoAlignableTransform;
class FaserActsGeometryContext;
class GeoAlignmentStore;

class FaserActsAlignmentCondAlg : public AthAlgorithm {

public:
  FaserActsAlignmentCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~FaserActsAlignmentCondAlg();

  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:

  SG::ReadCondHandleKey<GeoAlignmentStore> m_sctAlignStoreReadKey{
      this, "SCTAlignStoreReadKey", "SCTAlignmentStore", ""};

  SG::WriteCondHandleKey<FaserActsGeometryContext> m_wchk{
      this, "FaserActsAlignmentKey", "FaserActsAlignment", "cond handle key"};

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IFaserActsTrackingGeometrySvc> m_trackingGeometrySvc;

  const TrackerDD::SCT_DetectorManager *p_SCTManager;
};

#endif
