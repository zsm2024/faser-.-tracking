/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserGeoAdaptors/GeoScintHit.h"
#include "FaserGeoAdaptors/GeoFaserSiHit.h"
#include "FaserGeoAdaptors/GeoFaserCaloHit.h"
#include "FaserGeoAdaptors/GeoNeutrinoHit.h"

const NeutrinoDD::EmulsionDetectorManager *GeoNeutrinoHit::s_emulsion = 0;
const ScintDD::VetoDetectorManager      *GeoScintHit::s_veto = 0;
const ScintDD::VetoNuDetectorManager    *GeoScintHit::s_vetonu = 0;
const ScintDD::TriggerDetectorManager   *GeoScintHit::s_trigger = 0;
const ScintDD::PreshowerDetectorManager *GeoScintHit::s_preshower = 0;
const TrackerDD::SCT_DetectorManager    *GeoFaserSiHit::s_sct;
const CaloDD::EcalDetectorManager       *GeoFaserCaloHit::s_ecal = 0;
const EmulsionID                        *GeoNeutrinoHit::s_nID = 0;
const VetoID                            *GeoScintHit::s_vID = 0;
const VetoNuID                          *GeoScintHit::s_vnID = 0;
const TriggerID                         *GeoScintHit::s_tID = 0;
const PreshowerID                       *GeoScintHit::s_pID = 0;
const FaserSCT_ID                       *GeoFaserSiHit::s_sID = 0;
const EcalID                            *GeoFaserCaloHit::s_eID = 0;

