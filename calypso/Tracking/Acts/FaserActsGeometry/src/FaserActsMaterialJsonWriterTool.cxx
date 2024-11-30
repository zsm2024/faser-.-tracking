/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserActsMaterialJsonWriterTool.h"

#include "ActsInterop/Logger.h"


#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>

// FaserActsMaterialJsonWriterTool::FaserActsMaterialJsonWriterTool(const std::string& type, const std::string& name, const IInterface* parent) :
//   base_class(type, name, parent)
// {
// }
FaserActsMaterialJsonWriterTool::FaserActsMaterialJsonWriterTool(const std::string &type, const std::string &name,
                                const IInterface *parent)
  : base_class(type, name, parent)
{
}

StatusCode
FaserActsMaterialJsonWriterTool::initialize()
{
  ATH_MSG_INFO("Starting Material writer");

//  m_cfg.name = "FaserActsJsonGeometryConverter";
//  m_cfg.logger = makeActsAthenaLogger(this, "FaserActsJsonGeometryConverter");
  m_cfg.processSensitives = m_processSensitives;
  m_cfg.processApproaches = m_processApproaches;
  m_cfg.processRepresenting = m_processRepresenting;
  m_cfg.processBoundaries = m_processBoundaries;
  m_cfg.processVolumes = m_processVolumes;
  m_cfg.processDenseVolumes = m_processDenseVolumes;
  m_cfg.processNonMaterial = m_processNonMaterial;
  
  return StatusCode::SUCCESS;
}

void
FaserActsMaterialJsonWriterTool::write(const Acts::MaterialMapJsonConverter::DetectorMaterialMaps& detMaterial) const
{
  ATH_MSG_INFO("Start writing material");
  // Evoke the converter
  Acts::MaterialMapJsonConverter jmConverter(m_cfg, Acts::Logging::INFO);
  auto jout = jmConverter.materialMapsToJson(detMaterial);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}

void
FaserActsMaterialJsonWriterTool::write(const Acts::TrackingGeometry& tGeometry) const
{
  ATH_MSG_INFO("Start writing geometry");
  // Evoke the converter
  Acts::MaterialMapJsonConverter jmConverter(m_cfg, Acts::Logging::INFO);
  auto jout = jmConverter.trackingGeometryToJson(tGeometry);
  // And write the file
  std::ofstream ofj(m_filePath);
  ofj << std::setw(4) << jout << std::endl;
}
