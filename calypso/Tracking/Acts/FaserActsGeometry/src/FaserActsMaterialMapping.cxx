/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
   */

#include "FaserActsMaterialMapping.h"

// ATHENA
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsInterop/Logger.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Definitions/Units.hpp"

// PACKAGE
#include "FaserActsGeometry/FaserActsGeometryContext.h"
#include "FaserActsGeometryInterfaces/IFaserActsPropStepRootWriterSvc.h"

// STL
#include <fstream>
#include <string>

//TEST
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StandardAborters.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

FaserActsMaterialMapping::FaserActsMaterialMapping(const std::string &name,
    ISvcLocator *pSvcLocator)
  : AthReentrantAlgorithm(name, pSvcLocator),
  m_materialTrackWriterSvc("FaserActsMaterialTrackWriterSvc", name),
  m_inputMaterialStepCollection("MaterialStepRecords"),
  m_mappingState(m_gctx,m_mctx),
  m_mappingStateVol(m_gctx,m_mctx)
{}

StatusCode FaserActsMaterialMapping::initialize() {
  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  if(!m_mapSurfaces && !m_mapVolumes){
    ATH_MSG_ERROR("No element to map onto defined.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_materialTrackWriterSvc.retrieve() );
  if(m_mapSurfaces){
    ATH_CHECK(m_surfaceMappingTool.retrieve() );
    m_mappingState = m_surfaceMappingTool->mappingState();
  }
  if(m_mapVolumes){
    ATH_CHECK(m_volumeMappingTool.retrieve() );
    m_mappingStateVol = m_volumeMappingTool->mappingState();
  }
  ATH_CHECK(m_materialJsonWriterTool.retrieve() );
  ATH_CHECK(m_inputMaterialStepCollection.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode FaserActsMaterialMapping::execute(const EventContext &ctx) const {
  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  Acts::RecordedMaterialTrack mTrack;
  SG::ReadHandle<Trk::MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection, ctx);

  //make material track collection
  Trk::MaterialStepCollection::const_iterator iter = materialStepCollection->begin();
  Trk::MaterialStepCollection::const_iterator iter_end = materialStepCollection->end();
  Trk::MaterialStepCollection* newmaterialStepCollection= new Trk::MaterialStepCollection;
  //remove the material at Z>2500mm
  //need further study
  for(; iter != iter_end; ++iter){
    const Trk::MaterialStep* mat = *iter;
    Trk::MaterialStep* newmat  = const_cast<Trk::MaterialStep*>(mat);
    //remove the magnets, need to be updated
//    if((newmat->hitZ()<-1600&&newmat->hitZ()>-1920)||(newmat->hitZ()>-200&&newmat->hitZ()<100)||(newmat->hitZ()>1000&&newmat->hitZ()<1300)||(newmat->hitZ()>2000&&newmat->hitZ()<2500))
    if(newmat->hitZ()>-1920&&newmat->hitZ()<2500)
      newmaterialStepCollection->push_back(newmat);
  }
  if(newmaterialStepCollection->size()<1)
    return StatusCode::SUCCESS;

  /* test */
  std::vector<Acts::MaterialInteraction> nStep;
  Acts::RecordedMaterial recorded;
  double sum_X0 = 0;
  double sum_L0 = 0;

  double x_length = newmaterialStepCollection->back()->hitX() - newmaterialStepCollection->front()->hitX();
  double y_length = newmaterialStepCollection->back()->hitY() - newmaterialStepCollection->front()->hitY();
  double z_length = newmaterialStepCollection->back()->hitZ() - newmaterialStepCollection->front()->hitZ();

  double norm = 1/(std::sqrt(x_length*x_length +
	y_length*y_length +
	z_length*z_length));
  //set the initial z position to -4000
  double z_ini=-4000.;
  Acts::Vector3 v_pos{newmaterialStepCollection->front()->hitX()-(newmaterialStepCollection->front()->hitZ()-z_ini)*(newmaterialStepCollection->front()->hitX()-newmaterialStepCollection->back()->hitX())/(newmaterialStepCollection->front()->hitZ()-newmaterialStepCollection->back()->hitZ()),newmaterialStepCollection->front()->hitY()-(newmaterialStepCollection->front()->hitZ()-z_ini)*(newmaterialStepCollection->front()->hitY()-newmaterialStepCollection->back()->hitY())/(newmaterialStepCollection->front()->hitZ()-newmaterialStepCollection->back()->hitZ()),z_ini};
  Acts::Vector3 v_imp{x_length*norm, y_length*norm, z_length*norm};
  Acts::Vector3 prev_pos = v_pos;
  for(auto const step: *newmaterialStepCollection) {

    Acts::MaterialInteraction interaction;

    Acts::Vector3 pos{step->hitX(), step->hitY(), step->hitZ()};
    Acts::MaterialSlab matProp(Acts::Material::fromMassDensity(step->x0(), step->l0(), step->A(), step->Z(), (step->rho() * Acts::UnitConstants::g) ),step->steplength());
    interaction.position = pos;

    double x_dir = pos.x() - prev_pos.x();
    double y_dir = pos.y() - prev_pos.y();
    double z_dir = pos.z() - prev_pos.z();
    double norm_dir = 1/(std::sqrt(x_dir*x_dir +
	  y_dir*y_dir +
	  z_dir*z_dir));

    Acts::Vector3 dir{x_dir * norm_dir, y_dir * norm_dir, z_dir * norm_dir};
    interaction.direction = dir;
    prev_pos = pos;
    interaction.materialSlab = matProp;
    sum_X0 += step->steplengthInX0();
    sum_L0 += step->steplengthInL0();
    nStep.push_back(interaction);
  }
  recorded.materialInX0 = sum_X0;
  recorded.materialInL0 = sum_L0;
  recorded.materialInteractions = nStep;

  mTrack = std::make_pair(std::make_pair(v_pos, v_imp), recorded);

  if(m_mapSurfaces){
    ATH_MSG_INFO(name() << " start surface mapping");
    auto mappingState
      = const_cast<Acts::SurfaceMaterialMapper::State *>(&m_mappingState);
    auto context = m_surfaceMappingTool->trackingGeometryTool()->getNominalGeometryContext().context();
    std::reference_wrapper<const Acts::GeometryContext> geoContext(context);
    mappingState->geoContext = geoContext;

    m_surfaceMappingTool->mapper()->mapMaterialTrack(*mappingState, mTrack);
  }
  if(m_mapVolumes){
    ATH_MSG_INFO(name() << " start volume mapping");
    auto mappingStateVol
      = const_cast<Acts::VolumeMaterialMapper::State *>(&m_mappingStateVol);
    auto context = m_volumeMappingTool->trackingGeometryTool()->getNominalGeometryContext().context();
    std::reference_wrapper<const Acts::GeometryContext> geoContext(context);
    mappingStateVol->geoContext = geoContext;
    m_volumeMappingTool->mapper()->mapMaterialTrack(*mappingStateVol, mTrack);
  }
  m_materialTrackWriterSvc->write(mTrack);
  ATH_MSG_VERBOSE(name() << " execute done");
  return StatusCode::SUCCESS;
}

StatusCode FaserActsMaterialMapping::finalize() {

  Acts::DetectorMaterialMaps detectorMaterial;

  // Finalize all the maps using the cached state
  if(m_mapSurfaces && m_mapVolumes){
    m_surfaceMappingTool->mapper()->finalizeMaps(m_mappingState);
    m_volumeMappingTool->mapper()->finalizeMaps(m_mappingStateVol);
    // Loop over the state, and collect the maps for surfaces
    for (auto& [key, value] : m_mappingState.surfaceMaterial) {
      detectorMaterial.first.insert({key, std::move(value)});
    }
    // Loop over the state, and collect the maps for volumes
    for (auto& [key, value] : m_mappingStateVol.volumeMaterial) {
      detectorMaterial.second.insert({key, std::move(value)});
    }
  }
  else{
    if(m_mapSurfaces){
      m_surfaceMappingTool->mapper()->finalizeMaps(m_mappingState);
      // Loop over the state, and collect the maps for surfaces
      for (auto& [key, value] : m_mappingState.surfaceMaterial) {
	detectorMaterial.first.insert({key, std::move(value)});
      }
      // Loop over the state, and collect the maps for volumes
      for (auto& [key, value] : m_mappingState.volumeMaterial) {
	detectorMaterial.second.insert({key, std::move(value)});
      }
    }
    if(m_mapVolumes){
      m_volumeMappingTool->mapper()->finalizeMaps(m_mappingStateVol);
      // Loop over the state, and collect the maps for surfaces
      for (auto& [key, value] : m_mappingStateVol.surfaceMaterial) {
	detectorMaterial.first.insert({key, std::move(value)});
      }
      // Loop over the state, and collect the maps for volumes
      for (auto& [key, value] : m_mappingStateVol.volumeMaterial) {
	detectorMaterial.second.insert({key, std::move(value)});
      }
    }
  }

  m_materialJsonWriterTool->write(detectorMaterial);

  return StatusCode::SUCCESS;

}
