/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_FASERACTSMATERIALJSONWRITERTOOL_H
#define FASERACTSGEOMETRY_FASERACTSMATERIALJSONWRITERTOOL_H

// ATHENA
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/IInterface.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/

// PACKAGE
#include "FaserActsGeometryInterfaces/IFaserActsMaterialJsonWriterTool.h"

// ACTS
#include "Acts/Plugins/Json/MaterialMapJsonConverter.hpp"

namespace Acts {
  class TrackingGeometry;
}

class FaserActsMaterialJsonWriterTool : public extends<AthAlgTool, IFaserActsMaterialJsonWriterTool>
{

public:

  virtual StatusCode initialize() override;

  FaserActsMaterialJsonWriterTool(const std::string& type, const std::string& name,
                                const IInterface* parent);
  virtual ~FaserActsMaterialJsonWriterTool() = default;

  virtual
  void
  write(const Acts::MaterialMapJsonConverter::DetectorMaterialMaps& detMaterial) const override;

  virtual
  void
  write(const Acts::TrackingGeometry& tGeometry) const override;


private:

Acts::MaterialMapJsonConverter::Config m_cfg;

  Gaudi::Property<std::string> m_filePath{this, "OutputFile", "material-maps.json", "Output json file for the Material Map"};
  Gaudi::Property<bool> m_processSensitives{this, "processSensitives", true, "Write sensitive surface to the json file"};
  Gaudi::Property<bool> m_processApproaches{this, "processApproaches", true, "Write approche surface to the json file"};
  Gaudi::Property<bool> m_processRepresenting{this, "processRepresenting", true, "Write representing surface to the json file"};
  Gaudi::Property<bool> m_processBoundaries{this, "processBoundaries", true, "Write boundary surface to the json file"};
  Gaudi::Property<bool> m_processVolumes{this, "processVolumes", true, "Write volume to the json file"};
  Gaudi::Property<bool> m_processDenseVolumes{this, "processDenseVolumes", false, "Write dense volume to the json file"};
  Gaudi::Property<bool> m_processNonMaterial{this, "processnonmaterial", false, "Add proto material to all surfaces and volumes"};
};

#endif
