/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_IFASERACTSMATERIALJSONWRITERTOOL_H
#define ACTSGEOMETRY_IFASERACTSMATERIALJSONWRITERTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"

#include "Acts/Plugins/Json/MaterialMapJsonConverter.hpp"

namespace Acts {
  class TrackingGeometry;
}

class IFaserActsMaterialJsonWriterTool : virtual public IAlgTool {
public:

  DeclareInterfaceID(IFaserActsMaterialJsonWriterTool, 1, 0);

  virtual
  void
  write(const Acts::MaterialMapJsonConverter::DetectorMaterialMaps& detMaterial) const = 0;

  virtual
  void
  write(const Acts::TrackingGeometry& tGeometry) const = 0;

};


#endif
