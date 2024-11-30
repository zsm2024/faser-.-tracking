/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */
// ATHENA
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "TrackerReadoutGeometry/SCT_DetectorManager.h"

// PACKAGE
#include "FaserActsGeometry/FaserActsLayerBuilder.h"
#include "FaserActsGeometry/FaserActsDetectorElement.h"
#include "FaserActsGeometry/CuboidVolumeBuilder.h"
#include "ActsInterop/IdentityHelper.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoTube.h"

// ACTS
#include "Acts/Material/ProtoSurfaceMaterial.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Surfaces/PlaneSurface.hpp"
#include "Acts/Surfaces/DiscSurface.hpp"
#include "Acts/Surfaces/RectangleBounds.hpp"
#include "Acts/Geometry/GenericApproachDescriptor.hpp"
#include "Acts/Geometry/ApproachDescriptor.hpp"
#include "Acts/Geometry/ProtoLayer.hpp"
#include "Acts/Geometry/LayerCreator.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Definitions/Common.hpp"
#include "Acts/Geometry/PassiveLayerBuilder.hpp"
#include "Acts/Definitions/Algebra.hpp"
#include "Acts/Definitions/Units.hpp"
#include "Acts/Utilities/BinningType.hpp"

using Acts::Surface;
using Acts::Transform3;
using Acts::Translation3;

using namespace Acts::UnitLiterals;

FaserActs::CuboidVolumeBuilder::Config FaserActsLayerBuilder::buildVolume(const Acts::GeometryContext& gctx)
{
  FaserActs::CuboidVolumeBuilder::Config cvbConfig;
  std::vector<FaserActs::CuboidVolumeBuilder::VolumeConfig> volumeConfigs = {};

  //build the tracker stations 
  // iStation starts from 1 for the main detector (Faser-01 geometry) and from 0 if we include the IFT (Faser-02 geometry)
  auto siDetMng = static_cast<const TrackerDD::SCT_DetectorManager*>(m_cfg.mng);
  int nStations = siDetMng->numerology().numStations();

  for (int iStation=4-nStations; iStation<4; iStation++) {

    FaserActs::CuboidVolumeBuilder::VolumeConfig volumeConfig;

    std::vector<FaserActs::CuboidVolumeBuilder::LayerConfig> layerConfigs;
    layerConfigs.clear();

    for (int iPlane=0; iPlane<3; iPlane++) {

      m_cfg.station = iStation;
      m_cfg.plane = iPlane;
      std::vector<std::shared_ptr<const Acts::Surface>> surfaces;
      surfaces.clear();


      FaserActs::CuboidVolumeBuilder::SurfaceConfig surfacecfg;

      FaserActs::CuboidVolumeBuilder::LayerConfig layercfg;
      layercfg.binsX = 2;
      layercfg.binsY = 4;

      buildLayers(gctx, surfaces, layercfg, surfacecfg);

      layercfg.surfaceCfg = surfacecfg;
      layercfg.active = true;

      layercfg.surfaces = surfaces;
      layerConfigs.push_back(layercfg);

      if (iPlane == 1) {
	volumeConfig.position = Acts::Vector3(0, 0, surfacecfg.position.z()); 
      }
    }
    volumeConfig.length   = m_trackerDimensions;
    volumeConfig.layerCfg = layerConfigs;
    volumeConfig.name     = "Station_" + std::to_string(iStation);
    volumeConfigs.emplace_back(volumeConfig);

  }
  //build the veto/trigger stations 
  //first build veto station, then trigger station
  //veto station
  //Veto A, VetoRadiator, Veto B
  auto vetoManager = static_cast<const GeoVDetectorManager*>(m_cfg.vetomng);
  if(vetoManager!=nullptr){
  for(unsigned int i=0; i<vetoManager->getNumTreeTops(); i++){
    auto vol0 = vetoManager->getTreeTop(i)->getLogVol();
    //get the shape params and translation
    const GeoBox* shape = dynamic_cast<const GeoBox*>(vol0->getShape());
    auto trans = vetoManager->getTreeTop(i)->getX();
    if(vetoManager->getTreeTop(i)->getNChildVols()==0){
      volumeConfigs.emplace_back(buildScintVolume(gctx, trans.translation().x(),trans.translation().y(),trans.translation().z(), shape->getXHalfLength(),shape->getYHalfLength(),shape->getZHalfLength(),vol0->getName()));
    }
    else{
      for(size_t j =0 ;j< vetoManager->getTreeTop(i)->getNChildVols();j++){
	auto childtrans=vetoManager->getTreeTop(i)->getXToChildVol(j);
	const GeoBox* childshape = dynamic_cast<const GeoBox*>(vetoManager->getTreeTop(i)->getChildVol(j)->getLogVol()->getShape());
	volumeConfigs.emplace_back(buildScintVolume(gctx, trans.translation().x() + childtrans.translation().x(),trans.translation().y() + childtrans.translation().y(),trans.translation().z() + childtrans.translation().z(), childshape->getXHalfLength(), childshape->getXHalfLength(), childshape->getZHalfLength(),vol0->getName()+std::string("_")+std::to_string(j)));
      }
    }
  }
  }
  //build trigger stations
  auto triggerManager = static_cast<const GeoVDetectorManager*>(m_cfg.triggermng);
  if(triggerManager!=nullptr){
  for(unsigned int i=0; i<triggerManager->getNumTreeTops(); i++){
    auto vol0 = triggerManager->getTreeTop(i)->getLogVol();
    //get the shape params and translation
    const GeoBox* shape = dynamic_cast<const GeoBox*>(vol0->getShape());
    auto trans = triggerManager->getTreeTop(i)->getX();
    if(triggerManager->getTreeTop(i)->getNChildVols()==0){
      volumeConfigs.emplace_back(buildScintVolume(gctx,  trans.translation().x(), trans.translation().y(),trans.translation().z(), shape->getXHalfLength(), shape->getYHalfLength(), shape->getZHalfLength(),vol0->getName()));
    }
    else{
      for(size_t j =0 ;j< triggerManager->getTreeTop(i)->getNChildVols();j++){
	auto childtrans=triggerManager->getTreeTop(i)->getXToChildVol(j);
	const GeoBox* childshape = dynamic_cast<const GeoBox*>(triggerManager->getTreeTop(i)->getChildVol(j)->getLogVol()->getShape());
	volumeConfigs.emplace_back(buildScintVolume(gctx, trans.translation().x() + childtrans.translation().x(), trans.translation().y() + childtrans.translation().y(), trans.translation().z() + childtrans.translation().z(), childshape->getXHalfLength(), childshape->getYHalfLength(), childshape->getZHalfLength(),vol0->getName()+std::string("_")+std::to_string(j)));
      }
    }
  }
  }

  //build dipole magnets
  auto dipoleManager = static_cast<const GeoVDetectorManager*>(m_cfg.dipolemng);
  for(unsigned int i=0; i<dipoleManager->getNumTreeTops(); i++){
    auto vol0 = dipoleManager->getTreeTop(i)->getLogVol();
    //get the shape params and translation
    const GeoTube* shape = dynamic_cast<const GeoTube*>(vol0->getShape());
    auto trans = dipoleManager->getTreeTop(i)->getX();
    if(dipoleManager->getTreeTop(i)->getNChildVols()==0){
      volumeConfigs.emplace_back(buildDipoleVolume(gctx, trans.translation().z(), shape->getZHalfLength(), shape->getRMin(), shape->getRMax(), vol0->getName()+std::string("_")+std::to_string(i)));
    }
    else{
      for(size_t j =0 ;j< dipoleManager->getTreeTop(i)->getNChildVols();j++){
	auto childtrans=dipoleManager->getTreeTop(i)->getXToChildVol(j);
	const GeoTube* childshape = dynamic_cast<const GeoTube*>(dipoleManager->getTreeTop(i)->getChildVol(j)->getLogVol()->getShape());
	volumeConfigs.emplace_back(buildDipoleVolume(gctx, trans.translation().z() + childtrans.translation().z(), childshape->getZHalfLength(), childshape->getRMin(), childshape->getRMax(), vol0->getName()+std::string("_")+std::to_string(j)));
      }
    }
  }

  //sort the geometry by the  Z position, neccessary to have the correct boundary
  auto sortFunc = [](FaserActs::CuboidVolumeBuilder::VolumeConfig& v1, FaserActs::CuboidVolumeBuilder::VolumeConfig& v2){return v1.position.z()<v2.position.z();};
  std::sort(volumeConfigs.begin(),volumeConfigs.end(), sortFunc);
  //check volume positions
  if (logger().doPrint(Acts::Logging::VERBOSE)) {
    for(auto iter: volumeConfigs) ACTS_VERBOSE(" build volume centerred at Z = " << iter.position.z());
  }

  cvbConfig.position  = m_worldCenter;
  cvbConfig.length    = m_worldDimensions;
  cvbConfig.volumeCfg = volumeConfigs;

  return cvbConfig;
}

  void
FaserActsLayerBuilder::buildLayers(const Acts::GeometryContext& gctx,
    std::vector<std::shared_ptr<const Surface>>& surfaces,
    FaserActs::CuboidVolumeBuilder::LayerConfig& layercfg,
    FaserActs::CuboidVolumeBuilder::SurfaceConfig& surfacecfg)
{

  auto siDetMng = static_cast<const TrackerDD::SCT_DetectorManager*>(m_cfg.mng);

  for (int iRow = 0; iRow < 4; iRow++) {
    for (int iModule = -1; iModule < 2; iModule++) {
      for (int iSensor = 0; iSensor < 2; iSensor++) {

	if (iModule == 0) continue;
	const TrackerDD::SiDetectorElement* siDetElement = siDetMng->getDetectorElement(m_cfg.station, m_cfg.plane, iRow, iModule, iSensor) ;

	if (logger().doPrint(Acts::Logging::VERBOSE)) {
	  ACTS_VERBOSE("Found SCT sensor (" << m_cfg.station << "/" << m_cfg.plane << "/" << iRow << "/" << iModule << "/" << iSensor << ") with global center at (" << siDetElement->center().x() << ", " << siDetElement->center().y() << ", " << siDetElement->center().z() << ")." );
	}

	auto element = std::make_shared<const FaserActsDetectorElement>(siDetElement);

	surfaces.push_back(element->surface().getSharedPtr());

	m_cfg.elementStore->push_back(element);

	m_ModuleWidth = siDetElement->width();
	m_ModuleLength = siDetElement->length();
      }
    }
  }


  Acts::ProtoLayer pl(gctx, surfaces);

  if (logger().doPrint(Acts::Logging::VERBOSE)) {
    ACTS_VERBOSE(" Plane's zMin / zMax: " << pl.min(Acts::binZ) << " / " << pl.max(Acts::binZ));
  }

  std::shared_ptr<const Acts::ProtoSurfaceMaterial> materialProxy = nullptr;

  double layerZ = 0.5 * (pl.min(Acts::binZ) + pl.max(Acts::binZ));
  // increase layerThickness by 4 mm so that after alignment shifts all modules are still within the boundaries
  double layerThickness = (pl.max(Acts::binZ) - pl.min(Acts::binZ)) + 4_mm;
  double layerZInner = layerZ - layerThickness/2.;
  double layerZOuter = layerZ + layerThickness/2.;

  surfacecfg.position = Acts::Vector3(0, 0, layerZ);

  if (std::abs(layerZInner) > std::abs(layerZOuter)) std::swap(layerZInner, layerZOuter);

  auto rBounds = std::make_shared<const Acts::RectangleBounds>( 0.5*layercfg.binsY*m_ModuleWidth, 0.5*layercfg.binsX*m_ModuleLength ) ;

  Transform3 transformNominal(Translation3(0., 0., layerZ));

  Transform3 transformInner(Translation3(0., 0., layerZInner));

  Transform3 transformOuter(Translation3(0., 0., layerZOuter));

  std::shared_ptr<Acts::PlaneSurface> innerBoundary 
    = Acts::Surface::makeShared<Acts::PlaneSurface>(transformInner, rBounds);

  std::shared_ptr<Acts::PlaneSurface> nominalSurface 
    = Acts::Surface::makeShared<Acts::PlaneSurface>(transformNominal, rBounds);

  std::shared_ptr<Acts::PlaneSurface> outerBoundary 
    = Acts::Surface::makeShared<Acts::PlaneSurface>(transformOuter, rBounds);

  size_t matBinsX = layercfg.binsX*m_cfg.MaterialBins.first;
  size_t matBinsY = layercfg.binsY*m_cfg.MaterialBins.second;

  Acts::BinUtility materialBinUtil(
      matBinsX, -0.5*layercfg.binsY*m_ModuleWidth, 0.5*layercfg.binsY*m_ModuleWidth, Acts::open, Acts::binX);
  materialBinUtil += Acts::BinUtility(
      matBinsY, -0.5*layercfg.binsX*m_ModuleLength, 0.5*layercfg.binsX*m_ModuleLength, Acts::open, Acts::binY, transformInner);

  materialProxy
    = std::make_shared<const Acts::ProtoSurfaceMaterial>(materialBinUtil);

  ACTS_VERBOSE("[L] Layer is marked to carry support material on Surface ( "
      "inner=0 / center=1 / outer=2 ) : " << "inner");
  ACTS_VERBOSE("with binning: [" << matBinsX << ", " << matBinsY << "]");

  ACTS_VERBOSE("Created ApproachSurfaces for layer at:");
  ACTS_VERBOSE(" - inner:   Z=" << layerZInner);
  ACTS_VERBOSE(" - central: Z=" << layerZ);
  ACTS_VERBOSE(" - outer:   Z=" << layerZOuter);


  // set material on inner
  // @TODO: make this configurable somehow
  innerBoundary->assignSurfaceMaterial(materialProxy);


  std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;
  aSurfaces.push_back(std::move(innerBoundary));
  aSurfaces.push_back(std::move(nominalSurface));
  aSurfaces.push_back(std::move(outerBoundary));

  layercfg.approachDescriptor = new Acts::GenericApproachDescriptor(aSurfaces);

}


FaserActs::CuboidVolumeBuilder::VolumeConfig FaserActsLayerBuilder::buildDipoleVolume(const Acts::GeometryContext& gctx, double zpos, double zhalflength, double rmin, double rmax, std::string name){

  FaserActs::CuboidVolumeBuilder::VolumeConfig volumeConfig;

  std::vector<FaserActs::CuboidVolumeBuilder::LayerConfig> layerConfigs;
  layerConfigs.clear();

  FaserActs::CuboidVolumeBuilder::LayerConfig layercfg;
  FaserActs::CuboidVolumeBuilder::SurfaceConfig surfacecfg;
  surfacecfg.position = Acts::Vector3(0, 0, zpos);
  surfacecfg.thickness=zhalflength;

  std::vector<std::shared_ptr<const Acts::Surface>> surfaces;
  surfaces.clear();

  layercfg.binsX = 1;
  layercfg.binsY = 1;

  layercfg.active = true;

  //bounds are hard coded
  layercfg.surfaceCfg = surfacecfg;

  Transform3 transformCenter(Translation3(0., 0., (zpos)*1_mm));

  std::shared_ptr<Acts::DiscSurface> centerBoundary =
   Acts::Surface::makeShared<Acts::DiscSurface>(
   transformCenter, rmin, rmax);

  // @note: 0.001 mm is added to avoid overlapping surfaces and volume boundarys; 
  // This can cause issue in boundary intersecton check as any tiny overstepLimit is not allowed here:
  // https://github.com/acts-project/acts/blob/v32.0.2/Core/src/Geometry/TrackingVolume.cpp#L485 
  Transform3 transformInner(Translation3(0., 0., (zpos-zhalflength+0.001)*1_mm));

  std::shared_ptr<Acts::DiscSurface> innerBoundary =
   Acts::Surface::makeShared<Acts::DiscSurface>(
   transformInner, rmin, rmax);

  Transform3 transformOuter(Translation3(0., 0., (zpos+zhalflength)*1_mm));

  std::shared_ptr<Acts::DiscSurface> outerBoundary =
   Acts::Surface::makeShared<Acts::DiscSurface>(
   transformOuter, rmin, rmax);


//  size_t matBinsX = m_cfg.MaterialBins.first;
  size_t matBinsY = m_cfg.MaterialBins.second;
//  set bin size to 1 at phi and MaterialBins-Y at r direction
  Acts::BinUtility materialBinUtil(1, -M_PI, M_PI, Acts::closed, Acts::binPhi);
  materialBinUtil +=
    Acts::BinUtility(matBinsY, rmin, rmax, Acts::open, Acts::binR, transformInner);

  std::shared_ptr<const Acts::ProtoSurfaceMaterial> materialProxy =
    std::make_shared<const Acts::ProtoSurfaceMaterial>(materialBinUtil);


  innerBoundary->assignSurfaceMaterial(materialProxy);

  std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;
  aSurfaces.push_back(std::move(innerBoundary));
//  aSurfaces.push_back(std::move(centerBoundary));
//  aSurfaces.push_back(std::move(outerBoundary));
  Acts::ProtoLayer pl(gctx, aSurfaces);

  layercfg.surfaces = aSurfaces;
  layercfg.approachDescriptor = new Acts::GenericApproachDescriptor(aSurfaces);

  layerConfigs.push_back(layercfg);
  volumeConfig.position = Acts::Vector3(0, 0, zpos); 
  Acts::Vector3 length= { 500.0*1_mm, 500.0*1_mm, zhalflength*2.*1_mm };
  volumeConfig.length   = length;
  //volumeConfig.layerCfg = {};
  volumeConfig.layerCfg = layerConfigs;
  volumeConfig.name     = name;

  return volumeConfig;

}

FaserActs::CuboidVolumeBuilder::VolumeConfig FaserActsLayerBuilder::buildScintVolume(const Acts::GeometryContext& gctx, double xpos, double ypos, double zpos, double xhalflength, double yhalflength, double zhalflength, std::string name){

  FaserActs::CuboidVolumeBuilder::VolumeConfig volumeConfig;

  std::vector<FaserActs::CuboidVolumeBuilder::LayerConfig> layerConfigs;
  layerConfigs.clear();

  FaserActs::CuboidVolumeBuilder::LayerConfig layercfg;
  FaserActs::CuboidVolumeBuilder::SurfaceConfig surfacecfg;
  surfacecfg.position = Acts::Vector3(xpos, ypos, zpos);
  surfacecfg.thickness=zhalflength;

  std::vector<std::shared_ptr<const Acts::Surface>> surfaces;
  surfaces.clear();

  layercfg.binsX = 1;
  layercfg.binsY = 1;

  layercfg.active = true;

  //bounds are hard coded
  layercfg.surfaceCfg = surfacecfg;

  auto rBounds = std::make_shared<const Acts::RectangleBounds>( xhalflength*1_mm, yhalflength*1_mm) ;
//  surfacecfg.rBounds=rBounds;


  // @note: 0.001 mm is added to avoid overlapping surfaces and volume boundarys; 
  // This can cause issue in boundary intersecton check as any tiny overstepLimit is not allowed here:
  // https://github.com/acts-project/acts/blob/v32.0.2/Core/src/Geometry/TrackingVolume.cpp#L485 
  Transform3 transformInner(Translation3(xpos*1_mm, ypos*1_mm, (zpos-zhalflength+0.001)*1_mm));

  std::shared_ptr<Acts::PlaneSurface> innerBoundary =
   Acts::Surface::makeShared<Acts::PlaneSurface>(
   transformInner, rBounds);

  Transform3 transformCenter(Translation3(xpos*1_mm, ypos*1_mm, zpos*1_mm));
  std::shared_ptr<Acts::PlaneSurface> centerBoundary =
   Acts::Surface::makeShared<Acts::PlaneSurface>(
   transformCenter, rBounds);

  // @note: 0.001 mm is added to avoid overlapping surfaces and volume boundarys; 
  // This can cause issue in boundary intersecton check as any tiny overstepLimit is not allowed here:
  // https://github.com/acts-project/acts/blob/v32.0.2/Core/src/Geometry/TrackingVolume.cpp#L485 
  // @todo: since no material is mapped to trigger or dipole station, will remove the trigger/dipole station building in ACTS 
  Transform3 transformOuter(Translation3(xpos*1_mm, ypos*1_mm, (zpos+zhalflength-0.001)*1_mm));

  std::shared_ptr<Acts::PlaneSurface> outerBoundary =
   Acts::Surface::makeShared<Acts::PlaneSurface>(
   transformOuter, rBounds);

//  set bin size to 2 
  Acts::BinUtility materialBinUtil(2, (0.-xhalflength)*1_mm, xhalflength*1_mm, Acts::open, Acts::binX);
  materialBinUtil +=
    Acts::BinUtility(2, (0.-yhalflength)*1_mm, yhalflength*1_mm, Acts::open, Acts::binY, transformInner);

  std::shared_ptr<const Acts::ProtoSurfaceMaterial> materialProxy =
    std::make_shared<const Acts::ProtoSurfaceMaterial>(materialBinUtil);


  innerBoundary->assignSurfaceMaterial(materialProxy);

  std::vector<std::shared_ptr<const Acts::Surface>> aSurfaces;
  aSurfaces.push_back(std::move(innerBoundary));
  aSurfaces.push_back(std::move(centerBoundary));
  aSurfaces.push_back(std::move(outerBoundary));
  Acts::ProtoLayer pl(gctx, aSurfaces);

  layercfg.surfaces = aSurfaces;
  layercfg.approachDescriptor = new Acts::GenericApproachDescriptor(aSurfaces);

  layerConfigs.push_back(layercfg);
  volumeConfig.position = Acts::Vector3(0, 0, zpos); 
  Acts::Vector3 length= { 500.0*1_mm, 500.0*1_mm, zhalflength*2.*1_mm };
  volumeConfig.length   = length;
  //volumeConfig.layerCfg = {};
  volumeConfig.layerCfg = layerConfigs;
  volumeConfig.name     = name;


  return volumeConfig;

}
