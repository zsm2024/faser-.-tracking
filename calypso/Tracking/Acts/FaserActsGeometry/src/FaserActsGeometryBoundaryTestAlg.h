/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS and FASER collaborations
*/

#ifndef FASERACTSGEOMETRY_FASERACTSGEOMETRYBOUNDARYTESTALG_H
#define FASERACTSGEOMETRY_FASERACTSGEOMETRYBOUNDARYTESTALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"


class FaserActsGeometryBoundaryTestAlg : public AthReentrantAlgorithm {
public:
  FaserActsGeometryBoundaryTestAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext &ctx) const override final;
  virtual StatusCode finalize() override final;

private:
  ToolHandle<IFaserActsTrackingGeometryTool> m_trackingGeometryTool {
    this, "TrackingGeometryTool", "FaserActsTrackingGeometryTool"};
};


#endif // FASERACTSGEOMETRY_FASERACTSGEOMETRYBOUNDARYTESTALG_H