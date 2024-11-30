/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRYINTERFACES_IACTSEXTRAPOLATIONTOOL_H
#define FASERACTSGEOMETRYINTERFACES_IACTSEXTRAPOLATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"
#include "TrkEventPrimitives/ParticleHypothesis.h"

#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Propagator/MaterialInteractor.hpp"

#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "Acts/MagneticField/MagneticFieldContext.hpp"

/// Using some short hands for Recorded Material
using ActsRecordedMaterial = Acts::MaterialInteractor::result_type;
/// Finally the output of the propagation test
using ActsPropagationOutput =
        std::pair<std::vector<Acts::detail::Step>, ActsRecordedMaterial>;


namespace Acts {
  class TrackingGeometry;
}

class IFaserActsTrackingGeometryTool;


class IFaserActsExtrapolationTool : virtual public IAlgTool {
  public:

  DeclareInterfaceID(IFaserActsExtrapolationTool, 1, 0);

  virtual
  ActsPropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundTrackParameters& startParameters,
                   Acts::Direction navDir = Acts::Direction::Forward,
                   double pathLimit = std::numeric_limits<double>::max()) const = 0;
                   
  virtual
  std::optional<const Acts::CurvilinearTrackParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundTrackParameters& startParameters,
            Acts::Direction navDir = Acts::Direction::Forward,
            double pathLimit = std::numeric_limits<double>::max()) const = 0;
           
  virtual
  ActsPropagationOutput
  propagationSteps(const EventContext& ctx,
                   const Acts::BoundTrackParameters& startParameters,
                   const Acts::Surface& target,
                   Acts::Direction navDir = Acts::Direction::Forward,
                   double pathLimit = std::numeric_limits<double>::max()) const = 0;
            
  virtual
  std::optional<const Acts::BoundTrackParameters>
  propagate(const EventContext& ctx,
            const Acts::BoundTrackParameters& startParameters,
            const Acts::Surface& target,
            Acts::Direction navDir = Acts::Direction::Forward,
            double pathLimit = std::numeric_limits<double>::max()) const = 0;

  virtual
  const IFaserActsTrackingGeometryTool*
  trackingGeometryTool() const = 0;

  virtual 
  Acts::MagneticFieldContext getMagneticFieldContext(const EventContext& ctx) const = 0;
};

#endif
