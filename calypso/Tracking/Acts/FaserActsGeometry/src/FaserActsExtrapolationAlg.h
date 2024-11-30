/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERACTSGEOMETRY_ACTSEXTRAPOLATIONALG_H
#define FASERACTSGEOMETRY_ACTSEXTRAPOLATIONALG_H

// ATHENA
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "Gaudi/Property.h"  /*no forward decl: typedef*/
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Geometry/GeometryIdentifier.hpp"
#include "Acts/Utilities/Helpers.hpp"

// PACKAGE
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "MagFieldElements/FaserFieldCache.h"

// STL
#include <memory>
#include <vector>
#include <fstream>
#include <mutex>


namespace Acts {
  class TrackingGeometry;
  
  namespace detail {
    class Step;
  }
}

class IActsMaterialTrackWriterSvc;

class EventContext;
class IAthRNGSvc;
class IFaserActsExtrapolationTool;
class IFaserActsPropStepRootWriterSvc;

class FaserActsExtrapolationAlg : public AthReentrantAlgorithm {
public:
  FaserActsExtrapolationAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize() override;
  StatusCode execute(const EventContext& ctx) const override;
  StatusCode finalize() override;
  
private:
  ServiceHandle<IFaserActsPropStepRootWriterSvc> m_propStepWriterSvc;
  ServiceHandle<IAthRNGSvc> m_rndmGenSvc;

  ToolHandle<IFaserActsExtrapolationTool> m_extrapolationTool{this, "ExtrapolationTool", "FaserActsExtrapolationTool"};

  // poor-mans Particle Gun is included here right now
  Gaudi::Property<std::vector<double>> m_xRange{this, "XRange", {-150, 150}, ""};
  Gaudi::Property<std::vector<double>> m_yRange{this, "YRange", {-150, 150}, ""};
  Gaudi::Property<std::vector<double>> m_etaRange{this, "EtaRange", {4, 10}, ""};
  Gaudi::Property<std::vector<double>> m_ptRange{this, "PtRange", {0.1, 1000}, ""};
  Gaudi::Property<size_t> m_nParticlePerEvent{this, "NParticlesPerEvent", 1, "The number of particles per event"};

  // this does not work right now
  Gaudi::Property<bool> m_writeMaterialTracks{this, "WriteMaterialTracks", false, ""};
  ServiceHandle<IActsMaterialTrackWriterSvc> m_materialTrackWriterSvc;
  
  mutable std::mutex m_writeMutex{};
  mutable std::unique_ptr<std::ofstream> m_objOut;
  mutable size_t m_objVtxCount{0};

  void writeStepsObj(std::vector<Acts::detail::Step> steps) const;

};

#endif // ActsGeometry_ActsExtrapolation_h
