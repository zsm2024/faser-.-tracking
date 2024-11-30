/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_FASEERACTSLAYERBUILDER_H
#define FASERACTSGEOMETRY_FASEERACTSLAYERBUILDER_H

// PACKAGE
#include "FaserActsGeometry/FaserActsDetectorElement.h"

// ATHENA
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"

// ACTS
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/ILayerBuilder.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
#include "FaserActsGeometry/CuboidVolumeBuilder.h"

using namespace Acts::UnitLiterals;

class FaserActsTrackingGeomtrySvc;

namespace TrackerDD {
  class SCT_DetectorManager;
}

namespace Acts {
class Surface;
class LayerCreator;
class GeometryIdentifier;
}

class Identifier;

/// @class FaserActsLayerBuilder
class FaserActsLayerBuilder
{
public:
  /// @struct Config
  /// nested configuration struct for steering of the layer builder

  struct Config
  {
    /// string based identification
    std::string                          configurationName = "undefined";
    FaserActsDetectorElement::Subdetector subdetector
        = FaserActsDetectorElement::Subdetector::SCT;
    const TrackerDD::SCT_DetectorManager*   mng=nullptr;
    const GeoVDetectorManager*   vetomng=nullptr;
    const GeoVDetectorManager*   triggermng=nullptr;
    const GeoVDetectorManager*   dipolemng=nullptr;
    std::shared_ptr<const Acts::LayerCreator> layerCreator = nullptr;
    std::shared_ptr<std::vector<std::shared_ptr<const FaserActsDetectorElement>>> elementStore;
    std::shared_ptr<std::map<Identifier, Acts::GeometryIdentifier>> identifierMap;
    std::pair<size_t, size_t> MaterialBins = {30, 30};
    int station;
    int plane;
  };

  /// Constructor
  /// @param cfg is the configuration struct
  /// @param logger the local logging instance
  FaserActsLayerBuilder(const Config&                cfg,
                       std::unique_ptr<const Acts::Logger> logger
                       = Acts::getDefaultLogger("GMLayBldr", Acts::Logging::INFO))
    : m_logger(std::move(logger))
  {
    m_cfg = cfg;
  }

  /// Destructor
  ~FaserActsLayerBuilder() {}

  /// set the configuration object
  /// @param cfg is the configuration struct
  void
  setConfiguration(const Config& cfg);

  /// get the configuration object
  // Config
  // getConfiguration() const;

  FaserActsLayerBuilder::Config
  getConfiguration() const
  {
    return m_cfg;
  }

  const std::string&
  identification() const
  {
    return m_cfg.configurationName;
  }

  /// set logging instance
  void
  setLogger(std::unique_ptr<const Acts::Logger> logger);

  FaserActs::CuboidVolumeBuilder::Config
  buildVolume(const Acts::GeometryContext&);

  void
  buildLayers(const Acts::GeometryContext&, std::vector<std::shared_ptr<const Acts::Surface> >&, FaserActs::CuboidVolumeBuilder::LayerConfig&, FaserActs::CuboidVolumeBuilder::SurfaceConfig&);

private:

  FaserActs::CuboidVolumeBuilder::VolumeConfig buildScintVolume(const Acts::GeometryContext& ,double, double, double ,double, double,  double , std::string );
FaserActs::CuboidVolumeBuilder::VolumeConfig buildDipoleVolume(const Acts::GeometryContext& ,double , double , double , double , std::string );

  double m_ModuleWidth;
  double m_ModuleLength;
  /// configruation object
  Config m_cfg;
  Acts::Vector3 m_worldDimensions = { 600.0_mm, 600.0_mm, 8000.0_mm };
  Acts::Vector3 m_worldCenter = {0.0, 0.0, 0.0};
  Acts::Vector3 m_trackerDimensions = { 600.0_mm, 600.0_mm, 100.0_mm };

  /// Private access to the logger
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }

  std::vector<std::shared_ptr<const FaserActsDetectorElement>>
  getDetectorElements() const;

  /// logging instance
  std::unique_ptr<const Acts::Logger> m_logger;

  ///// @todo make clear where the FaserActsDetectorElement lives
  //std::vector<std::shared_ptr<const FaserActsDetectorElement>> m_elementStore;

};


#endif
