/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsExtrapolationTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/PhysicalConstants.h"

// PACKAGE
// #include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometrySvc.h"
// #include "FaserActsGeometryInterfaces/IFaserActsTrackingGeometryTool.h"
#include "ActsInterop/Logger.h"

// ACTS
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/BoundaryCheck.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"

// BOOST
#include <boost/variant/variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>

// STL
#include <iostream>
#include <memory>

namespace ActsExtrapolationDetail {
  using VariantPropagatorBase = boost::variant<
    Acts::Propagator<Acts::EigenStepper<>, Acts::Navigator>,
    Acts::Propagator<Acts::EigenStepper<>, Acts::Navigator>
  >;

  class VariantPropagator : public VariantPropagatorBase
  {
  public:
    using VariantPropagatorBase::VariantPropagatorBase;
  };

}


using ActsExtrapolationDetail::VariantPropagator;



FaserActsExtrapolationTool::FaserActsExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
{
}


FaserActsExtrapolationTool::~FaserActsExtrapolationTool()
{
}



StatusCode
FaserActsExtrapolationTool::initialize()
{
  using namespace std::literals::string_literals;


  ATH_MSG_INFO("Initializing ACTS extrapolation");

  m_logger = Acts::getDefaultLogger("ExtrapolationTool", Acts::Logging::ERROR);
//  m_logger = makeActsAthenaLogger(this, "Prop", "FaserActsExtrapTool");

  ATH_CHECK( m_trackingGeometryTool.retrieve() );
  std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry
    = m_trackingGeometryTool->trackingGeometry();

  Acts::Navigator navigator( Acts::Navigator::Config{ trackingGeometry } );

  if (m_fieldMode == "FASER") {
    ATH_MSG_INFO("Using FASER magnetic field service");
    ATH_CHECK( m_fieldCondObjInputKey.initialize() );
    auto bField = std::make_shared<FASERMagneticFieldWrapper>();
    auto stepper = Acts::EigenStepper<>(std::move(bField));
    auto propagator = Acts::Propagator<decltype(stepper), Acts::Navigator>(std::move(stepper),
                                                                      std::move(navigator), Acts::getDefaultLogger("ExtrapolationTool", Acts::Logging::ERROR));
    m_varProp = std::make_unique<VariantPropagator>(propagator);
  }
  else if (m_fieldMode == "Constant") {
    if (m_constantFieldVector.value().size() != 3)
    {
      ATH_MSG_ERROR("Incorrect field vector size. Using empty field.");
      return StatusCode::FAILURE; 
    }
    
    Acts::Vector3 constantFieldVector = Acts::Vector3(m_constantFieldVector[0], 
                                                      m_constantFieldVector[1], 
                                                      m_constantFieldVector[2]);

    ATH_MSG_INFO("Using constant magnetic field: (Bx, By, Bz) = (" << m_constantFieldVector[0] << ", " 
                                                                   << m_constantFieldVector[1] << ", " 
                                                                   << m_constantFieldVector[2] << ")");
    auto bField = std::make_shared<Acts::ConstantBField>(constantFieldVector);
    auto stepper = Acts::EigenStepper<>(std::move(bField));
    auto propagator = Acts::Propagator<decltype(stepper), Acts::Navigator>(std::move(stepper),
                                                                      std::move(navigator));
    m_varProp = std::make_unique<VariantPropagator>(propagator);
  }
  
  ATH_MSG_INFO("ACTS extrapolation successfully initialized");
  return StatusCode::SUCCESS;
}


ActsPropagationOutput
FaserActsExtrapolationTool::propagationSteps(const EventContext& ctx,
                                        const Acts::BoundTrackParameters& startParameters,
                                        Acts::Direction navDir /*= Acts::Direction::Forward*/,
                                        double pathLimit /*= std::numeric_limits<double>::max()*/) const
  {
    using namespace Acts::UnitLiterals;
    ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

    Acts::MagneticFieldContext mctx = getMagneticFieldContext(ctx);
    const FaserActsGeometryContext& gctx
      = m_trackingGeometryTool->getGeometryContext(ctx);

    auto anygctx = gctx.context();

    // Action list and abort list
    using ActionList = Acts::ActionList<SteppingLogger, Acts::MaterialInteractor>;
    using AbortConditions = Acts::AbortList<EndOfWorld>;
    
    using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;
    
    Options options(anygctx, mctx);
    options.pathLimit = pathLimit;

    options.loopProtection
      = (Acts::VectorHelpers::perp(startParameters.momentum())
          < m_ptLoopers * 1_MeV);

    options.maxStepSize = m_maxStepSize * 1_m;
    options.maxSteps = m_maxStep;
    options.direction = navDir;

    auto& mInteractor = options.actionList.get<Acts::MaterialInteractor>();
    mInteractor.multipleScattering = m_interactionMultiScatering;
    mInteractor.energyLoss = m_interactionEloss;
    mInteractor.recordInteractions = m_interactionRecord;

    ActsPropagationOutput output;
    
    auto res = boost::apply_visitor([&](const auto& propagator) -> ResultType {
        auto result = propagator.propagate(startParameters, options);
        if (!result.ok()) {
          return result.error();
        }
        auto& propRes = *result;

        auto steppingResults = propRes.template get<SteppingLogger::result_type>();
        auto materialResult = propRes.template get<Acts::MaterialInteractor::result_type>();
        output.first = std::move(steppingResults.steps);
        output.second = std::move(materialResult);
        // try to force return value optimization, not sure this is necessary
        return std::move(output);
    }, *m_varProp);

    if (!res.ok()) {
      ATH_MSG_ERROR("Got error during propagation: "
  		  << res.error() << " " << res.error().message()
                    << ". Returning empty step vector.");
      return {};
    }
    output = std::move(*res);

    ATH_MSG_VERBOSE("Collected " << output.first.size() << " steps");
    if(output.first.size() == 0) {
      ATH_MSG_WARNING("ZERO steps returned by stepper, that is not typically a good sign");
    }

    ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " end");

    return output;
  }


std::optional<const Acts::CurvilinearTrackParameters>
FaserActsExtrapolationTool::propagate(const EventContext& ctx,
                                 const Acts::BoundTrackParameters& startParameters,
                                 Acts::Direction navDir /*= Acts::forward*/,
                                 double pathLimit /*= std::numeric_limits<double>::max()*/) const
{
  using namespace Acts::UnitLiterals;
  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

  Acts::MagneticFieldContext mctx = getMagneticFieldContext(ctx);
  const FaserActsGeometryContext& gctx
      = m_trackingGeometryTool->getGeometryContext(ctx);

  auto anygctx = gctx.context();

  // Action list and abort list
  using ActionList = Acts::ActionList<Acts::MaterialInteractor>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;
  using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;

  Options options(anygctx, mctx);
  options.pathLimit = pathLimit;

  options.loopProtection
    = (Acts::VectorHelpers::perp(startParameters.momentum())
       < m_ptLoopers * 1_MeV);
  options.maxStepSize = m_maxStepSize * 1_m;
  options.maxSteps = m_maxStep;
  options.direction = navDir;

  auto& mInteractor = options.actionList.get<Acts::MaterialInteractor>();
  mInteractor.multipleScattering = m_interactionMultiScatering;
  mInteractor.energyLoss = m_interactionEloss;
  mInteractor.recordInteractions = m_interactionRecord;

  auto parameters = boost::apply_visitor([&](const auto& propagator) -> std::optional<const Acts::CurvilinearTrackParameters> {
      auto result = propagator.propagate(startParameters, options);
      if (!result.ok()) {
        ATH_MSG_WARNING("Got error during propagation:" << result.error()
        << ". Returning empty parameters.");
        return std::nullopt;
      }
      return result.value().endParameters;
    }, *m_varProp);
  
  return parameters;
}

ActsPropagationOutput
FaserActsExtrapolationTool::propagationSteps(const EventContext& ctx,
                                        const Acts::BoundTrackParameters& startParameters,
                                        const Acts::Surface& target,
                                        Acts::Direction navDir /*= Acts::forward*/,
                                        double pathLimit /*= std::numeric_limits<double>::max()*/) const
{
  using namespace Acts::UnitLiterals;
  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

  Acts::MagneticFieldContext mctx = getMagneticFieldContext(ctx);
  const FaserActsGeometryContext& gctx
    //= m_trackingGeometryTool->getGeometryContext(ctx);
    = m_trackingGeometryTool->getGeometryContext(ctx );

  auto anygctx = gctx.context();

  // Action list and abort list
  using ActionList = Acts::ActionList<SteppingLogger, Acts::MaterialInteractor>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;
  using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;

  Options options(anygctx, mctx);
  options.pathLimit = pathLimit;

  options.loopProtection
    = (Acts::VectorHelpers::perp(startParameters.momentum())
       < m_ptLoopers * 1_MeV);
  options.maxStepSize = m_maxStepSize * 1_m;
  options.maxSteps = m_maxStep;
  options.direction = navDir;

  auto& mInteractor = options.actionList.get<Acts::MaterialInteractor>();
  mInteractor.multipleScattering = m_interactionMultiScatering;
  mInteractor.energyLoss = m_interactionEloss;
  mInteractor.recordInteractions = m_interactionRecord;

  ActsPropagationOutput output;

  auto res = boost::apply_visitor([&](const auto& propagator) -> ResultType {
      auto result = propagator.propagate(startParameters, target, options);
      if (!result.ok()) {
        return result.error();
      }
      auto& propRes = *result;

      auto steppingResults = propRes.template get<SteppingLogger::result_type>();
      auto materialResult = propRes.template get<Acts::MaterialInteractor::result_type>();
      output.first = std::move(steppingResults.steps);
      output.second = std::move(materialResult);
      return std::move(output);
    }, *m_varProp);

  if (!res.ok()) {
    ATH_MSG_ERROR("Got error during propagation:" << res.error() << res.error().message()
                  << ". Returning empty step vector.");
    return {};
  }
  output = std::move(*res);

  ATH_MSG_VERBOSE("Collected " << output.first.size() << " steps");
  if (output.first.size() == 0) {
     ATH_MSG_WARNING("ZERO steps returned by stepper, that is not typically a good sign");
  }

  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " end");

  return output;
}

std::optional<const Acts::BoundTrackParameters>
FaserActsExtrapolationTool::propagate(const EventContext& ctx,
                                 const Acts::BoundTrackParameters& startParameters,
                                 const Acts::Surface& target,
                                 Acts::Direction navDir /*= Acts::forward*/,
                                 double pathLimit /*= std::numeric_limits<double>::max()*/) const
{
  using namespace Acts::UnitLiterals;
  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__ << " begin");

  Acts::MagneticFieldContext mctx = getMagneticFieldContext(ctx);
  const FaserActsGeometryContext& gctx = m_trackingGeometryTool->getGeometryContext(ctx);

  auto anygctx = gctx.context();

  // Action list and abort list
  using ActionList = Acts::ActionList<Acts::MaterialInteractor>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;
  using Options = Acts::PropagatorOptions<ActionList, AbortConditions>;

  Options options(anygctx, mctx);
  options.pathLimit = pathLimit;

  options.loopProtection
    = (Acts::VectorHelpers::perp(startParameters.momentum())
       < m_ptLoopers * 1_MeV);
  options.maxStepSize = m_maxStepSize * 1_m;
  options.maxSteps = m_maxStep;
  options.direction = navDir;

  auto& mInteractor = options.actionList.get<Acts::MaterialInteractor>();
  mInteractor.multipleScattering = m_interactionMultiScatering;
  mInteractor.energyLoss = m_interactionEloss;
  mInteractor.recordInteractions = m_interactionRecord;

  auto parameters = boost::apply_visitor([&](const auto& propagator) -> std::optional<const Acts::BoundTrackParameters> {
      auto result = propagator.propagate(startParameters, target, options);
      if (!result.ok()) {
        ATH_MSG_ERROR("Got error during propagation: " << result.error()
        << ". Returning empty parameters.");
        return std::nullopt; 
      }
      return result.value().endParameters;
    }, *m_varProp);

  return parameters;
}

Acts::MagneticFieldContext FaserActsExtrapolationTool::getMagneticFieldContext(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserFieldCacheCondObj> readHandle{m_fieldCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
     std::stringstream msg;
     msg << "Failed to retrieve magnetic field condition data " << m_fieldCondObjInputKey.key() << ".";
     throw std::runtime_error(msg.str());
  }
  const FaserFieldCacheCondObj* fieldCondObj{*readHandle};

  return Acts::MagneticFieldContext(fieldCondObj);
}
