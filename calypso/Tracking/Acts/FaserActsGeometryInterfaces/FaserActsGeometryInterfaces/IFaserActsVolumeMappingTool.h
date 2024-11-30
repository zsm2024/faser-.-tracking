/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRYINTERFACES_IACTSVOLUMEMAPPINGTOOL_H
#define ACTSGEOMETRYINTERFACES_IACTSVOLUMEMAPPINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

#include "Acts/Material/VolumeMaterialMapper.hpp"

class IFaserActsTrackingGeometryTool;

class IFaserActsVolumeMappingTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IFaserActsVolumeMappingTool, 1, 0);


  virtual
  std::shared_ptr<Acts::VolumeMaterialMapper>
  mapper() const = 0;

  virtual
  Acts::VolumeMaterialMapper::State
  mappingState() const = 0;

  virtual
  const IFaserActsTrackingGeometryTool*
  trackingGeometryTool() const = 0;

};

#endif
