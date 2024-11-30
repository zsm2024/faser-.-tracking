/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsExtrapolationAlg.h"

// ATHENA
#include "AthenaKernel/RNGWrapper.h"
#include "AthenaKernel/IAthRNGSvc.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PhysicalConstants.h"

// ACTS
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "ActsInterop/Logger.h"

// PACKAGE
#include "ActsGeometryInterfaces/IActsMaterialTrackWriterSvc.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "FaserActsGeometryInterfaces/IFaserActsExtrapolationTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "FaserActsGeometryInterfaces/IFaserActsPropStepRootWriterSvc.h"

// OTHER
#include "CLHEP/Random/RandomEngine.h"

// STL
#include <string>
#include <fstream>

using namespace Acts::UnitLiterals;

FaserActsExtrapolationAlg::FaserActsExtrapolationAlg(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_propStepWriterSvc("FaserActsPropStepRootWriterSvc", name),
      m_rndmGenSvc("AthRNGSvc", name),
      m_materialTrackWriterSvc("FaserActsMaterialTrackWriterSvc", name)
{
}

StatusCode FaserActsExtrapolationAlg::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  ATH_CHECK( m_rndmGenSvc.retrieve() );
  ATH_CHECK( m_extrapolationTool.retrieve() );
  ATH_CHECK( m_propStepWriterSvc.retrieve() );

  if (m_writeMaterialTracks) {
   ATH_CHECK( m_materialTrackWriterSvc.retrieve() );
  }

  m_objOut = std::make_unique<std::ofstream>("steps.obj");

  return StatusCode::SUCCESS;
}

StatusCode FaserActsExtrapolationAlg::execute(const EventContext& ctx) const
{

  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);

  ATH_MSG_VERBOSE("Extrapolating " << m_nParticlePerEvent << " particles");

  for (size_t i = 0; i < m_nParticlePerEvent; i++) {

//  double d0 = 0;
//  double z0 = 0;
  double phi = rngEngine->flat() * 2*M_PI - M_PI;
  std::vector<double> etaRange = m_etaRange;
  std::vector<double> xRange = m_xRange;
  std::vector<double> yRange = m_yRange;
  double etaMin = etaRange.at(0);
  double etaMax = etaRange.at(1);
  double eta = rngEngine->flat() * std::abs(etaMax - etaMin) + etaMin;

  double xMin = xRange.at(0);
  double xMax = xRange.at(1);
  double yMin = yRange.at(0);
  double yMax = yRange.at(1);
  double x0 = rngEngine->flat() * std::abs(xMax - xMin) + xMin;
  double y0 = rngEngine->flat() * std::abs(yMax - yMin) + yMin;

  std::vector<double> ptRange = m_ptRange;
  double ptMin = ptRange.at(0) * 1_GeV;
  double ptMax = ptRange.at(1) * 1_GeV;

  double pt = rngEngine->flat() * std::abs(ptMax - ptMin) + ptMin;

  Acts::Vector3 momentum(
      pt * std::cos(phi), pt * std::sin(phi), pt * std::sinh(eta));

  double theta = Acts::VectorHelpers::theta(momentum);

  double charge = rngEngine->flat() > 0.5 ? -1 : 1;

  double qop =  charge / momentum.norm();

  std::shared_ptr<Acts::PerigeeSurface> surface
    = Acts::Surface::makeShared<Acts::PerigeeSurface>(Acts::Vector3(0, 0, 0));

  double t = 0;

  Acts::BoundVector pars;
  pars << x0, y0, phi, theta, qop, t;
  std::optional<Acts::BoundMatrix> cov = std::nullopt;

  if(charge != 0.) {
      // Perigee, no alignment -> default geo context
      FaserActsGeometryContext gctx
        = m_extrapolationTool->trackingGeometryTool()->getNominalGeometryContext();
      auto anygctx = gctx.context();
      Acts::GenericBoundTrackParameters startParameters(std::move(surface), std::move(pars), std::move(cov), Acts::ParticleHypothesis::pion());
      auto output = m_extrapolationTool->propagationSteps(ctx, startParameters);
      m_propStepWriterSvc->write(output.first);
      writeStepsObj(output.first);
      if(m_writeMaterialTracks){
	Acts::RecordedMaterialTrack track;
	track.first.first = Acts::Vector3(0,0,0);
	track.first.second = momentum;
	track.second = std::move(output.second);
	m_materialTrackWriterSvc->write(track);
      }
  }


  ATH_MSG_VERBOSE(name() << " execute done");
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserActsExtrapolationAlg::finalize() {
  return StatusCode::SUCCESS;
}

void FaserActsExtrapolationAlg::writeStepsObj(std::vector<Acts::detail::Step> steps) const
{
  std::lock_guard<std::mutex> lock(m_writeMutex);

  std::ofstream& out = *m_objOut;
  std::stringstream lstr;
  lstr << "l";
  for(const auto& step : steps) {
    const auto& pos = step.position;
    out << "v " << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
    lstr << " " << m_objVtxCount;
    m_objVtxCount++;
  }

  lstr << std::endl;

  out << lstr.str() << std::endl;
}
