/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelInterfaces/IGeoDbTagSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include <string>
#include <iostream>

DecodeFaserVersionKey::DecodeFaserVersionKey(const IGeoModelSvc * geoModel, const std::string & node)
{
  defineTag<IGeoModelSvc>(geoModel,node);
}

DecodeFaserVersionKey::DecodeFaserVersionKey(const IGeoDbTagSvc * geoDbTag, const std::string & node)
{
  defineTag<IGeoDbTagSvc>(geoDbTag,node);
}

DecodeFaserVersionKey::DecodeFaserVersionKey(const std::string & node)
{
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IGeoDbTagSvc* geoDbTag{nullptr};
  if(svcLocator->service("GeoDbTagSvc",geoDbTag).isFailure())
    throw std::runtime_error("DecodeFaserVersionKey constructor: cannot access GeoDbTagSvc");
  defineTag<IGeoDbTagSvc>(geoDbTag,node);
}

template <class T>
void DecodeFaserVersionKey::defineTag(const T* svc, const std::string & node)
{
  std::string nodeOverrideTag;
  std::string scintOverrideTag; // Scint, tracker and Calo has two levels.
  std::string trackerOverrideTag;
  std::string caloOverrideTag;
  std::string neutrinoOverrideTag;
  if (node == "FASER") {
    nodeOverrideTag = "";
  } else if (node == "Neutrino") {
    nodeOverrideTag = svc->neutrinoVersionOverride();
  } else if (node == "Emulsion") {
    neutrinoOverrideTag = svc->neutrinoVersionOverride();
    nodeOverrideTag = svc->emulsionVersionOverride();    
  } else if (node == "Scintillator") {
    nodeOverrideTag = svc->scintVersionOverride();
  } else if (node == "Veto") {
    scintOverrideTag = svc->scintVersionOverride();
    nodeOverrideTag = svc->vetoVersionOverride();
  } else if (node == "VetoNu") {
    scintOverrideTag = svc->scintVersionOverride();
    nodeOverrideTag = svc->vetoNuVersionOverride();
  } else if (node == "Trigger") {
    scintOverrideTag = svc->scintVersionOverride();
    nodeOverrideTag = svc->triggerVersionOverride();
  } else if (node == "Preshower") {
    scintOverrideTag = svc->scintVersionOverride();
    nodeOverrideTag = svc->preshowerVersionOverride();
  } else if (node == "Tracker") {
    nodeOverrideTag  = svc->trackerVersionOverride();
  } else if (node == "SCT") {
    nodeOverrideTag = svc->sctVersionOverride();
    trackerOverrideTag = svc->trackerVersionOverride();
  } else if (node == "Dipole") {
    nodeOverrideTag = svc->dipoleVersionOverride();
    trackerOverrideTag = svc->trackerVersionOverride();
  } else if (node == "Calorimeter") {
    nodeOverrideTag  = svc->caloVersionOverride();
  } else if (node == "Ecal") {
    caloOverrideTag = svc->caloVersionOverride();
    nodeOverrideTag = svc->ecalVersionOverride();
  } else if (node == "Trench") {
    nodeOverrideTag = svc->trenchVersionOverride();
  } else {
    std::cout << "DecodeFaserVersionKey passed an unknown node:" << node << std::endl; 
    nodeOverrideTag = "";
  }
  // Default to faser version
  m_tag = svc->faserVersion();
  m_node = "FASER";
  
  // If neutrinoOverrideTag is specified (and is not just "CUSTOM") then override with the neutrino tag.
  std::string neutrinoTag;
  if (!neutrinoOverrideTag.empty()) {
    // We dont care about the return value (custom = true/false). We only take notice of the custom 
    // flag if the override is at the node we have selected. Ie we only look at nodeOverrideTag 
    // in order to set m_custom. At any rate, we have to remove the CUSTOM string if it is present.
    getCustomTag(neutrinoOverrideTag, neutrinoTag);
  }
  if (!neutrinoTag.empty()) {
    m_tag = neutrinoTag;
    m_node = "Neutrino";
  }

  // If scintOverrideTag is specified (and is not just "CUSTOM") then override with the scint tag.
  std::string scintTag;
  if (!scintOverrideTag.empty()) {
    // We dont care about the return value (custom = true/false). We only take notice of the custom 
    // flag if the override is at the node we have selected. Ie we only look at nodeOverrideTag 
    // in order to set m_custom. At any rate, we have to remove the CUSTOM string if it is present.
    getCustomTag(scintOverrideTag, scintTag);
  }
  if (!scintTag.empty()) {
    m_tag = scintTag;
    m_node = "Scintillator";
  }

  // If trackerOverrideTag is specified (and is not just "CUSTOM") then override with the tracker tag.
  std::string trackerTag;
  if (!trackerOverrideTag.empty()) {
    // We dont care about the return value (custom = true/false). We only take notice of the custom 
    // flag if the override is at the node we have selected. Ie we only look at nodeOverrideTag 
    // in order to set m_custom. At any rate, we have to remove the CUSTOM string if it is present.
    getCustomTag(trackerOverrideTag, trackerTag);
  }
  if (!trackerTag.empty()) {
    m_tag = trackerTag;
    m_node = "Tracker";
  }

  // If caloOverrideTag is specified (and is not just "CUSTOM") then override with the calo tag.
  std::string caloTag;
  if (!caloOverrideTag.empty()) {
    // We dont care about the return value (custom = true/false). We only take notice of the custom 
    // flag if the override is at the node we have selected. Ie we only look at nodeOverrideTag 
    // in order to set m_custom. At any rate, we have to remove the CUSTOM string if it is present.
    getCustomTag(caloOverrideTag, caloTag);
  }
  if (!caloTag.empty()) {
    m_tag = caloTag;
    m_node = "Calorimeter";
  }

  // Finally if subsystem tag is overriden then use that.
  std::string outputTag;
  m_custom = getCustomTag(nodeOverrideTag, outputTag);

  if (!outputTag.empty()) {
    m_tag  = outputTag;
    m_node = node;
  }

}

const std::string &
DecodeFaserVersionKey::tag() const
{
  return m_tag;
}

const std::string &
DecodeFaserVersionKey::node() const
{
  return m_node;
}

bool
DecodeFaserVersionKey::custom() const
{
  return m_custom;
}



bool 
DecodeFaserVersionKey::getCustomTag(const std::string & inputTag, std::string & outputTag)
{
  //
  // Check if CUSTOM is specified. If it is not specified then outputTag = inputTag.
  // If the tag is just "CUSTOM" then set output tag to "" so that we use the higher level tag.
  // If the tag is of the form CUSTOM-XXXXX use the XXXXX as the tag
  // The separating character (in this example a '-') can be any character. 
  //
  bool custom = false;
  outputTag = inputTag;
  if (!inputTag.empty()) {
    if (inputTag.substr(0,6) == "CUSTOM") {
      custom = true;
      // If its CUSTOM-something skip the next character and get the something
      outputTag = inputTag.substr(6);
      if (!outputTag.empty()) outputTag = outputTag.substr(1);
    }
  }
  return custom;
}

