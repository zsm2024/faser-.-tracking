/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Base class
#include "G4AtlasTools/DetectorGeometryBase.h"
#include "FaserGeoDetectorTool.h"
#include "FaserGeo2G4Builder.h"
#include "FaserVolumeBuilder.h"

#include "G4NistManager.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4GDMLParser.hh"

// Geant4 includes used in functions

FaserGeoDetectorTool::FaserGeoDetectorTool(const std::string& type, const std::string& name, const IInterface* parent)
  : DetectorGeometryBase(type,name,parent),m_blParamOn(false),m_blGetTopTransform(true),m_geoDetectorName("")
{
  m_topTransform.setIdentity();
  ATH_MSG_DEBUG( "FaserGeoDetectorTool constructor for " << name );
  declareProperty("GeoDetectorName",m_geoDetectorName, "Name of the detector in GeoModel, if different from G4.");
  declareProperty("GDMLFileOut",m_dumpGDMLFile,"File name where the GDML description for the detector will be dumped.");

}

StatusCode FaserGeoDetectorTool::initialize()
{
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::initialize(): Starting" );
  if(m_detectorName.empty())
    {
      m_detectorName = this->name();
      // re-initialize m_detectorName in order to take the real detector name rather than the path to it
      size_t ipos=m_detectorName.value().find_last_of('.');
      size_t length=m_detectorName.value().size();
      if (ipos<length)
        {
          ATH_MSG_VERBOSE( "m_detectorName: " << m_detectorName.value() << " needs to be reset.");
          m_detectorName = m_detectorName.value().substr(ipos+1,length-ipos-1);
          ATH_MSG_VERBOSE( "m_detectorName default value reset to " << m_detectorName.value());
        }
    }
  ATH_MSG_DEBUG( name() << "FaserGeoDetectorTool::initialize() : Detector name = " << m_detectorName.value() );
  if(m_geoDetectorName.empty())
    {
      m_geoDetectorName = m_detectorName.value();
    }
  ATH_MSG_DEBUG( name() << "FaserGeoDetectorTool::initialize() : Geo Detector name = " << m_geoDetectorName );

  ATH_CHECK(m_geo2G4Svc.retrieve());
  m_builderName = m_geo2G4Svc->GetDefaultBuilder()->GetKey();
  m_blGetTopTransform = m_geo2G4Svc->UseTopTransforms();
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::initialize(): Finished" );
  return StatusCode::SUCCESS;
}


void FaserGeoDetectorTool::BuildGeometry()
{
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::BuildGeometry(): Starting" );
  // ATH_MSG_ALWAYS("BuildGeometry with CurrentDetectorName: " << m_notifierSvc->GetCurrentDetectorName());
  G4LogicalVolume* temp = this->Convert();
  m_envelope.theEnvelope=temp;

  if (this->IsTopTransform())
    {
      this->SetInitialTransformation();
    }
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::BuildGeometry(): Finished" );
}

void FaserGeoDetectorTool::PositionInParent()
{
  ATH_MSG_DEBUG( name() << " FaserGeoDetectorTool::PositionInParent(): Starting" );

  DetectorGeometryBase::PositionInParent();
  if (!m_dumpGDMLFile.empty()) {
    G4GDMLParser parser;
    parser.Write(m_dumpGDMLFile,m_envelope.thePositionedVolume);
  }
}

G4LogicalVolume* FaserGeoDetectorTool::Convert()
{
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::Convert(): Starting" );
  ATH_MSG_DEBUG( name() << " FaserGeoDetectorTool::Convert(): GeoDetectorName:builderName = "<<m_geoDetectorName<<":"<<m_builderName);
  FaserGeo2G4Builder Builder(m_geoDetectorName);
  FaserVolumeBuilder *b=Builder.GetVolumeBuilder(m_builderName);
  b->SetParam(m_blParamOn);
  G4LogicalVolume *temp=Builder.BuildTree();
  if(this->IsTopTransform())
    {
      m_topTransform = Builder.GetDetectorTransform();
    }

  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::Convert(): Finished" );
  return temp;
}

bool FaserGeoDetectorTool::IsTopTransform()
{
  return m_blGetTopTransform;
}

void FaserGeoDetectorTool::SetInitialTransformation()
{
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::SetInitialTransformation(): Starting" );
  if (!m_envelope.theRotation)
    {
      ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::SetInitialTransformation(): Creating new G4RotationMatrix" );
      m_envelope.theRotation=new G4RotationMatrix;
    }
  *(m_envelope.theRotation)=m_topTransform.getRotation().inverse();
  m_envelope.thePosition=m_topTransform.getTranslation();
  ATH_MSG_VERBOSE( name() << " FaserGeoDetectorTool::SetInitialTransformation(): Finished" );
}

