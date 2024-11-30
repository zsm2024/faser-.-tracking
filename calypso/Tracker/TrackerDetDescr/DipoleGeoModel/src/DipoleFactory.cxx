/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DipoleGeoModel/DipoleFactory.h"

#include "DipoleGeoModel/DipoleDataBase.h"
#include "DipoleGeoModel/DipoleGeneralParameters.h"
#include "DipoleGeoModel/DipoleWrappingParameters.h"

// Extra material
#include "TrackerGeoModelUtils/DistortedMaterialManager.h"
#include "TrackerGeoModelUtils/ExtraMaterial.h"

// Material manager
#include "TrackerGeoModelUtils/TrackerMaterialManager.h"

// GeoModel includes
#include "GeoModelKernel/GeoNameTag.h"  
#include "GeoModelKernel/GeoPhysVol.h"  
#include "GeoModelKernel/GeoTube.h"  
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoIdentifierTag.h"
#include "GeoModelKernel/GeoTransform.h"
// #include "GeoModelKernel/GeoAlignableTransform.h"  
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoDefinitions.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GeoModelInterfaces/StoredMaterialManager.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"

// RDBAccessSvc includes
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

// StoreGate includes
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/PhysicalConstants.h"

#include <iostream>

DipoleFactory::DipoleFactory(const TrackerDD::AthenaComps * athenaComps)
  : TrackerDD::DetectorFactoryBase(athenaComps),
    m_manager(0)
{  
    // create a new det manager
    m_manager = new TrackerDD::DipoleManager();

    m_db = new DipoleDataBase(athenaComps);
}


DipoleFactory::~DipoleFactory()
{
    delete m_db;
}



//## Other Operations (implementation)
void DipoleFactory::create(GeoPhysVol *world )
{

  // Get the material manager:  
  DecodeFaserVersionKey dipoleVersionKey(geoDbTagSvc(),"Dipole");
  DecodeFaserVersionKey trackerVersionKey(geoDbTagSvc(),"Tracker");
  DecodeFaserVersionKey faserVersionKey(geoDbTagSvc(),"FASER");


  msg(MSG::DEBUG) << "Building Dipoles with FASER Version Tag: " << faserVersionKey.tag() << endmsg;
  msg(MSG::DEBUG) << "                                with Tracker Version Tag: " << trackerVersionKey.tag() << " at Node: "
      << trackerVersionKey.node() << endmsg;
  msg(MSG::DEBUG) << " Dipole Version " << rdbAccessSvc()->getChildTag("Dipole", dipoleVersionKey.tag(), dipoleVersionKey.node(), "FASERDD") << endmsg;

//   IRDBRecordset_ptr scalingTable = rdbAccessSvc()->getRecordsetPtr("DipoleScaling", trackerVersionKey.tag(), trackerVersionKey.node(), "FASERDD");

  TrackerMaterialManager * materialManager = new TrackerMaterialManager("DipoleMaterialManager", getAthenaComps());
//   materialManager->addScalingTable(scalingTable);

  auto parameters = std::make_unique<DipoleGeneralParameters>(m_db);
  auto wrappingParameters = std::make_unique<DipoleWrappingParameters>(m_db);

  const GeoMaterial* air = materialManager->getMaterial("std::Air");
  const GeoMaterial* NdFeB = materialManager->getMaterial(parameters->material());
  const GeoMaterial* upstreamMaterial = materialManager->getMaterial(wrappingParameters->material1());
  const GeoMaterial* downstreamMaterial = materialManager->getMaterial(wrappingParameters->material2());
  double upstreamThickness = wrappingParameters->thickness1();
  double downstreamThickness = wrappingParameters->thickness2();
  double wrappingRadius = wrappingParameters->radius();
  msg(MSG::ALWAYS) << "Upstream wrapping thickness: " << upstreamThickness << "; Downstream wrapping thickness: " << downstreamThickness << endmsg;
  msg(MSG::ALWAYS) << "Upstream wrapping position: " << -(parameters->longLength() + upstreamThickness)/2 << "; Downstream wrapping position: " << (parameters->longLength() + downstreamThickness)/2 << endmsg;

  GeoTrf::Transform3D dipoleTransform = parameters->partTransform("Dipole");
  const GeoTube* shortShape = new GeoTube(parameters->innerRadius(), parameters->outerRadius(), parameters->shortLength()/2);
  GeoLogVol* shortLog = new GeoLogVol("ShortDipole", shortShape, NdFeB);
  const GeoTube* longShape = new GeoTube(parameters->innerRadius(), parameters->outerRadius(), parameters->longLength()/2);
  GeoLogVol* longLog = new GeoLogVol("LongDipole", longShape, NdFeB);
  // const GeoTube* wrappedShape = new GeoTube(parameters->innerRadius(), std::max(parameters->outerRadius(), wrappingRadius), parameters->longLength()/2 + std::max(upstreamThickness, downstreamThickness));
  const GeoTube* wrappedShape = new GeoTube(0.0, std::max(parameters->outerRadius(), wrappingRadius), parameters->longLength()/2 + std::max(upstreamThickness, downstreamThickness));
  GeoLogVol* wrappedLog = new GeoLogVol("WrappedLongDipole", wrappedShape, air);
  GeoPhysVol* wrappedPV = new GeoPhysVol(wrappedLog);
  GeoPhysVol* longPV = new GeoPhysVol(longLog);
  wrappedPV->add(longPV);
  const GeoTube* upstreamShape = new GeoTube(0.0, wrappingRadius, upstreamThickness/2);
  GeoLogVol* upstreamLog = new GeoLogVol("UpstreamWrapping", upstreamShape, upstreamMaterial);
  GeoPhysVol* upstreamPV = new GeoPhysVol(upstreamLog);
  wrappedPV->add(new GeoTransform(GeoTrf::Translate3D(0.0, 0.0, -(parameters->longLength() + upstreamThickness)/2)));
  wrappedPV->add(upstreamPV);

  const GeoTube* downstreamShape = new GeoTube(0.0, wrappingRadius, downstreamThickness/2);
  GeoLogVol* downstreamLog = new GeoLogVol("DownstreamWrapping", downstreamShape, downstreamMaterial);
  GeoPhysVol* downstreamPV = new GeoPhysVol(downstreamLog);
  wrappedPV->add(new GeoTransform(GeoTrf::Translate3D(0.0, 0.0, (parameters->longLength() + downstreamThickness)/2)));
  wrappedPV->add(downstreamPV);

  std::vector<std::string> partNames {"UpstreamDipole", "CentralDipole", "DownstreamDipole"};
  for (int station = 0; station < 3; station++)
  {
    if (parameters->partPresent(partNames[station]))
    {
      world->add(new GeoNameTag("Dipole"));
      world->add(new GeoIdentifierTag(station));
      GeoTransform* stationTransform = new GeoTransform(dipoleTransform * parameters->partTransform(partNames[station]));
      world->add(stationTransform);
      GeoPhysVol* dipolePV {nullptr};
      if (station == 0)
      {
        // dipolePV = new GeoPhysVol(longLog);
        dipolePV = wrappedPV;
      }
      else
      {
        dipolePV = new GeoPhysVol(shortLog);
      }
      world->add(dipolePV);
      m_manager->addTreeTop(dipolePV);
    }
  }
  // Extra material
//   TrackerDD::DistortedMaterialManager xMatManager;
//   TrackerDD::ExtraMaterial xMat(&xMatManager);
//   xMat.add(ServPhys, "InDetServMat");

  delete materialManager;
}


const TrackerDD::DipoleManager* DipoleFactory::getDetectorManager () const {
  return m_manager;
}

