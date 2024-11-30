/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_RandomDisabledCellGenerator.h"

#include "FaserSiDigitization/SiChargedDiodeCollection.h"

#include "FaserSiDigitization/SiHelper.h"

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

// constructor
FaserSCT_RandomDisabledCellGenerator::FaserSCT_RandomDisabledCellGenerator(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent)
{
}

StatusCode FaserSCT_RandomDisabledCellGenerator::initialize() {
  ATH_MSG_DEBUG("FaserSCT_RandomDisabledCellGenerator::initialize()");
  ATH_MSG_INFO("\tCreating missing bond generator with "<<m_disableProbability<<" probability");
  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_RandomDisabledCellGenerator::finalize() {
  ATH_MSG_INFO("FaserSCT_RandomDisabledCellGenerator::finalize()");
  return StatusCode::SUCCESS;
}

// process the collection 
void FaserSCT_RandomDisabledCellGenerator::process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const {
  // disabling is applied to all cells even unconnected or below threshold ones to be able to use these cells as well
  // loop on all charged diodes
  for (std::pair<const TrackerDD::SiCellId, SiChargedDiode>& chargedDiode: collection) {
    if (CLHEP::RandFlat::shoot(rndmEngine)<m_disableProbability) {
      SiHelper::disconnected(chargedDiode.second, true, false);
    }
  }
}
