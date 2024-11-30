/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsTrackingGeometrySvc.h"

// ATHENA
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/EventContext.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
//#include "DipoleGeoModel/DipoleManager.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelFaserUtilities/GeoModelExperiment.h"

// ACTS
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Geometry/ITrackingVolumeBuilder.hpp"
#include "Acts/Geometry/LayerArrayCreator.hpp"
#include "Acts/Geometry/SurfaceArrayCreator.hpp"
#include "Acts/Geometry/LayerCreator.hpp"
#include "Acts/Geometry/TrackingVolumeArrayCreator.hpp"
#include "Acts/Geometry/CylinderVolumeHelper.hpp"
#include "Acts/Geometry/TrackingGeometryBuilder.hpp"
#include "Acts/Geometry/CylinderVolumeBuilder.hpp"
//#include "Acts/Geometry/CuboidVolumeBuilder.hpp"
#include "FaserActsGeometry/CuboidVolumeBuilder.h"
#include "Acts/ActsVersion.hpp"
#include "Acts/Definitions/Units.hpp"
#include <Acts/Plugins/Json/JsonMaterialDecorator.hpp>
#include <Acts/Plugins/Json/MaterialMapJsonConverter.hpp>

// PACKAGE
#include "FaserActsGeometry/FaserActsLayerBuilder.h"
#include "FaserActsGeometry/FaserActsDetectorElement.h"
#include "FaserActsGeometry/FaserActsAlignmentStore.h"
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "ActsInterop/IdentityHelper.h"
#include "ActsInterop/Logger.h"

using namespace Acts::UnitLiterals;

FaserActsTrackingGeometrySvc::FaserActsTrackingGeometrySvc(const std::string& name, ISvcLocator* svc)
   : base_class(name,svc),
   m_detStore("StoreGateSvc/DetectorStore", name)
{
  m_elementStore = std::make_shared<std::vector<std::shared_ptr<const FaserActsDetectorElement>>>();
  m_identifierMap = std::make_shared<std::map<Identifier, Acts::GeometryIdentifier>>();
}

StatusCode
FaserActsTrackingGeometrySvc::initialize()
{
  ATH_MSG_INFO(name() << " is initializing");
  ATH_MSG_INFO("Acts version is: v" << Acts::VersionMajor << "."
                                    << Acts::VersionMinor << "."
                                    << Acts::VersionPatch
                                    << " [" << Acts::CommitHash << "]");

  ATH_CHECK ( m_detStore->retrieve(p_SCTManager, "SCT") );

  //get all the subdetectors
  const GeoModelExperiment* theExpt = nullptr;
  ATH_CHECK( m_detStore->retrieve( theExpt ));
  const GeoVDetectorManager *vetoManager = theExpt->getManager("Veto");
  const GeoVDetectorManager *triggerManager = theExpt->getManager("Trigger");
  const GeoVDetectorManager *dipoleManager = theExpt->getManager("Dipole");

  Acts::LayerArrayCreator::Config lacCfg;
  auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
      lacCfg, makeActsAthenaLogger(this, std::string("LayArrCrtr"), std::string("ActsTGSvc")));

  Acts::TrackingVolumeArrayCreator::Config tvcCfg;
  auto trackingVolumeArrayCreator
      = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
          tvcCfg, makeActsAthenaLogger(this, std::string("TrkVolArrCrtr"), std::string("ActsTGSvc")));

  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator          = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;

  auto cylinderVolumeHelper
    = std::make_shared<const Acts::CylinderVolumeHelper>(
        cvhConfig, makeActsAthenaLogger(this, std::string("CylVolHlpr"), std::string("ActsTGSvc")));

  Acts::TrackingGeometryBuilder::Config tgbConfig;

  if (m_useMaterialMap) {
    std::shared_ptr<const Acts::IMaterialDecorator> matDeco = nullptr;
    std::string matFile = m_materialMapInputFile;
    ATH_MSG_INFO("Configured to use material input: " << matFile);
    if (matFile.find(".json") != std::string::npos) {
      // Set up the converter first
      Acts::MaterialMapJsonConverter::Config jsonGeoConvConfig;
      // Set up the json-based decorator
      matDeco = std::make_shared<const Acts::JsonMaterialDecorator>(
          jsonGeoConvConfig, m_materialMapInputFile, Acts::Logging::INFO);
    }
    tgbConfig.materialDecorator = matDeco;
  }

  try {
    // SCT
    tgbConfig.trackingVolumeBuilders.push_back([&](
	  const auto& gctx, const auto& /*inner*/, const auto&) {
	auto tv = makeVolumeBuilder(gctx, p_SCTManager, vetoManager, triggerManager, dipoleManager);
	return tv->trackingVolume(gctx);
	});
  }
  catch (const std::invalid_argument& e) {
    ATH_MSG_ERROR(e.what());
    return StatusCode::FAILURE;
  }


  auto trackingGeometryBuilder
    = std::make_shared<const Acts::TrackingGeometryBuilder>(tgbConfig,
	makeActsAthenaLogger(this, std::string("TrkGeomBldr"), std::string("ActsTGSvc")));


  // default geometry context, this is nominal
  FaserActsGeometryContext constructionContext;
  constructionContext.construction = true;

  m_trackingGeometry = trackingGeometryBuilder
    ->trackingGeometry(constructionContext.context());

  ATH_MSG_VERBOSE("Printing info of ACTS tracking geometry surface:");
  m_trackingGeometry->visitSurfaces([&](const Acts::Surface* surface) {
      auto center = surface->center(constructionContext.context());
      auto rotation = surface->transform(constructionContext.context()).rotation();
      ATH_MSG_VERBOSE("ACTS surface " <<  surface->geometryId() << " has center " << center.transpose() << ", local X axis at " << rotation.col(0).transpose() <<", local Z axis at " << rotation.col(2).transpose());
    } 
  ); 

  ATH_MSG_VERBOSE("Building nominal alignment store");
  FaserActsAlignmentStore* nominalAlignmentStore = new FaserActsAlignmentStore();
  ATH_MSG_VERBOSE("finish Building nominal alignment store");

  populateAlignmentStore(nominalAlignmentStore);
  ATH_MSG_VERBOSE("finish alignment population");
  populateIdentifierMap(m_identifierMap.get());

  ATH_MSG_VERBOSE("get alignment store");
  // manage ownership
  m_nominalAlignmentStore = std::unique_ptr<const FaserActsAlignmentStore>(nominalAlignmentStore);

  ATH_MSG_INFO("Acts TrackingGeometry construction completed");

  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
FaserActsTrackingGeometrySvc::trackingGeometry() {

  ATH_MSG_VERBOSE("Retrieving tracking geometry");
  return m_trackingGeometry;
}

std::shared_ptr<const Acts::ITrackingVolumeBuilder>
FaserActsTrackingGeometrySvc::makeVolumeBuilder(const Acts::GeometryContext& gctx, const TrackerDD::SCT_DetectorManager* manager, const GeoVDetectorManager* vetoManager, const GeoVDetectorManager* triggerManager, const GeoVDetectorManager* dipoleManager)
{
  std::string managerName = manager->getName();

  std::shared_ptr<FaserActsLayerBuilder> gmLayerBuilder;

  FaserActsLayerBuilder::Config cfg;
  cfg.subdetector = FaserActsDetectorElement::Subdetector::SCT;
  cfg.mng = static_cast<const TrackerDD::SCT_DetectorManager*>(manager);
  //get veto and trigger manager
  if(vetoManager!=nullptr)
    cfg.vetomng = static_cast<const GeoVDetectorManager*>(vetoManager);
  else
    ATH_MSG_WARNING(name() << " can not find veto stations");
  if(triggerManager!=nullptr)
    cfg.triggermng = static_cast<const GeoVDetectorManager*>(triggerManager);
  else
    ATH_MSG_WARNING(name() << " can not find trigger stations");
  if(dipoleManager!=nullptr)
    cfg.dipolemng = static_cast<const GeoVDetectorManager*>(dipoleManager);
  else
    ATH_MSG_WARNING(name() << " can not find dipoles");
  cfg.elementStore = m_elementStore;
  cfg.identifierMap = m_identifierMap;
  std::vector<size_t> matBins(m_MaterialBins);
  cfg.MaterialBins = { matBins.at(0), matBins.at(1) };
  gmLayerBuilder = std::make_shared<FaserActsLayerBuilder>(cfg,
      makeActsAthenaLogger(this, std::string("GMLayBldr"), std::string("ActsTGSvc")));
  auto cvbConfig = gmLayerBuilder->buildVolume(gctx);
  auto cvb = std::make_shared<const FaserActs::CuboidVolumeBuilder>(cvbConfig);

  return cvb;
}

void
FaserActsTrackingGeometrySvc::populateAlignmentStore(FaserActsAlignmentStore *store) const
{
  // populate the alignment store with all detector elements
  m_trackingGeometry->visitSurfaces(
      [store](const Acts::Surface* srf) {
      const Acts::DetectorElementBase* detElem = srf->associatedDetectorElement();
      if(detElem){
      const auto* gmde = dynamic_cast<const FaserActsDetectorElement*>(detElem);
      gmde->storeTransform(store);
      }
      });
}

const FaserActsAlignmentStore*
FaserActsTrackingGeometrySvc::getNominalAlignmentStore() const
{
  return m_nominalAlignmentStore.get();
}

void
FaserActsTrackingGeometrySvc::populateIdentifierMap(IdentifierMap *map) const
{
  m_trackingGeometry->visitSurfaces(
    [map](const Acts::Surface* srf) {
    const Acts::DetectorElementBase* detElem = srf->associatedDetectorElement();
    if(detElem){
    const auto* faserDetElem = dynamic_cast<const FaserActsDetectorElement*>(detElem);
    map->insert(std::make_pair(faserDetElem->identify(), srf->geometryId()));
    }
  });
}

std::shared_ptr<IdentifierMap>
FaserActsTrackingGeometrySvc::getIdentifierMap() const
{
  return m_identifierMap;
}
