/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/WriteCondHandleKey.h"

// PACKAGE

// STL
#include <string>

class IFaserActsTrackingGeometrySvc;
class FaserActsAlignmentStore;

class FaserActsGeometryContext;

/// @class FaserNominalAlignmentCondAlg
/// Conditions algorithm which produces an (effectively)
/// infinitely valid FaserActsAlignmentStore which has
/// nominal alignments (= identity deltas)
///
class FaserNominalAlignmentCondAlg : public AthAlgorithm {

public:
  FaserNominalAlignmentCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~FaserNominalAlignmentCondAlg();

  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  SG::WriteCondHandleKey<FaserActsGeometryContext> m_wchk{
      this, "FaserActsAlignmentKey", "FaserActsAlignment", "cond handle key"};

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IFaserActsTrackingGeometrySvc> m_trackingGeometrySvc;
};
