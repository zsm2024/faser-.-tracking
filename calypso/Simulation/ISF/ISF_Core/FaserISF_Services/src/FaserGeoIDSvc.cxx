/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeoIDSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "FaserGeoIDSvc.h"

// framework includes
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// DetectorDescription
#include "FaserDetDescr/FaserRegionHelper.h"

// Geant4
#include "G4Navigator.hh"
#include "G4TransportationManager.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TouchableHistoryHandle.hh"

// STL includes
#include <algorithm>

/** Constructor **/
ISF::FaserGeoIDSvc::FaserGeoIDSvc(const std::string& name,ISvcLocator* svc) :
  base_class(name,svc), m_navigator { nullptr }
{ }


/** Destructor **/
ISF::FaserGeoIDSvc::~FaserGeoIDSvc()
{ 
  if (m_navigator != nullptr)
  {
    delete m_navigator;
    m_navigator = nullptr;
  }
}


// Athena algtool's Hooks
StatusCode  ISF::FaserGeoIDSvc::initialize()
{
  ATH_MSG_INFO("initialize() ...");

  G4VPhysicalVolume* world = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
  if (world == nullptr)
  {
    ATH_MSG_FATAL("Unable to retrieve Geant4 world volume.");
    return StatusCode::FAILURE;
  }

  m_navigator = new G4Navigator();
  if (m_navigator == nullptr)
  {
    ATH_MSG_FATAL("Unable to create private navigator");
    return StatusCode::FAILURE;
  }
  m_navigator->SetWorldVolume(world);

  //  ATH_MSG_INFO("initialize() successful");
  return StatusCode::SUCCESS;
}


StatusCode  ISF::FaserGeoIDSvc::finalize() {
  ATH_MSG_INFO("finalize() ...");

  // ATH_MSG_INFO("finalize() successful");
  return StatusCode::SUCCESS;
}


ISF::InsideType ISF::FaserGeoIDSvc::inside(const Amg::Vector3D& pos, FaserDetDescr::FaserRegion geoID) const 
{

  // an arbitrary unitary direction with +1 in x,y,z
  // (following this direction will cross any FaserRegion boundary if position is close to it in the first place)
  const Amg::Vector3D dir(1., 1., 1.);
  const Amg::Vector3D dirUnit( dir.unit() );

  // create particle positions which are a bit forward and a bit aft of the current position
  const Amg::Vector3D posFwd( pos + dirUnit*m_tolerance );
  const Amg::Vector3D posAft( pos - dirUnit*m_tolerance );

  FaserDetDescr::FaserRegion geoIDFwd = identifyGeoID(posFwd);
  FaserDetDescr::FaserRegion geoIDAft = identifyGeoID(posAft);

  // default case: particle is outside given geoID
  ISF::InsideType where = ISF::fOutside;

  // only if either the fwd or the aft step is inside the given geoID,
  // inside/surface cases need to be resolved
  if ( (geoID == geoIDFwd) || (geoID == geoIDAft) ) {
    // 1. inside
    if ( geoIDFwd == geoIDAft ) {
      where = ISF::fInside;
      // 2. surface
    } else if ( geoIDFwd != geoIDAft ) {
      where = ISF::fSurface;
    }
  }

  return where;
}

FaserDetDescr::FaserRegion ISF::FaserGeoIDSvc::identifyGeoID(const Amg::Vector3D& pos) const 
{
  m_navigator->LocateGlobalPointAndSetup(G4ThreeVector(pos.x(), pos.y(), pos.z()));
  G4TouchableHistoryHandle pHistory = m_navigator->CreateTouchableHistoryHandle();
  if (pHistory->GetHistoryDepth() <= 0) return FaserDetDescr::fFaserCavern;

  // Search upward through volume hierarchy until we find something intelligible
  for (G4int level = 0; level <= pHistory->GetHistoryDepth(); level++)
  {
    G4String lvName = pHistory->GetVolume(level)->GetLogicalVolume()->GetName();

    if (lvName == "SCT::SCT" || lvName == "SCT::Station") return FaserDetDescr::fFaserTracker;

    if (lvName == "Veto::Veto"           || lvName == "Veto::VetoStationA"          ||
        lvName == "Trigger::Trigger"     || lvName == "Trigger::TriggerStationA"    ||
        lvName == "Preshower::Preshower" || lvName == "Preshower::PreshowerStationA" ||
        lvName == "VetoNu::VetoNu"       || lvName == "VetoNu::VetoNuStationA" ) return FaserDetDescr::fFaserScintillator;
    
    if (lvName == "Ecal::Ecal") return FaserDetDescr::fFaserCalorimeter;

    if (lvName == "Emulsion::Emulsion" || lvName == "Emulsion::EmulsionStationA" ) return FaserDetDescr::fFaserNeutrino;

    if (lvName == "Dipole::Dipole") return FaserDetDescr::fFaserDipole;

    if (lvName == "Trench::Trench") return FaserDetDescr::fFaserTrench;
  }

  // If all else fails
  return FaserDetDescr::fFaserCavern;
}


FaserDetDescr::FaserRegion ISF::FaserGeoIDSvc::identifyNextGeoID(const Amg::Vector3D& pos,
                                                            const Amg::Vector3D& dir) const 
{
  FaserDetDescr::FaserRegion geoID = identifyGeoID( pos + dir.unit()*m_tolerance );

  return geoID;
}

