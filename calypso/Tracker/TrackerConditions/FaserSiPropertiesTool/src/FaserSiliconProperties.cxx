/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSiPropertiesTool/FaserSiliconProperties.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <iostream>

namespace Tracker {

using TrackerDD::CarrierType;
using TrackerDD::holes;
using TrackerDD::electrons;

// This value for the number of eh pairs per deposited energy is fairly standard I think. 
// In reality there is some temperture dependence but for the temperature ranges we deal with 
// I don't think variations are too signifcant.
const double FaserSiliconProperties::s_ehPairsPerEnergyDefault = 1. / (3.62 * CLHEP::eV); // 1 eh pair per 3.62 CLHEP::eV.

// Constants used in the formula
const double elecHallFactZero  = 1.13;
const double elecHallFact_drdt = 8e-4;
const double elecV_sat_0       = 1.53e9 * CLHEP::cm/CLHEP::s;
const double elecV_sat_exp     = -0.87;
const double elecE_crit_0      = 1.01   * CLHEP::volt/CLHEP::cm;
const double elecE_crit_exp    = 1.55;
const double elecBeta_0        = 2.57e-2;
const double elecBeta_exp      = 0.66;

const double holeHallFactZero  = 0.72;
const double holeHallFact_drdt = -5e-4;
const double holeV_sat_0       = 1.62e8 * CLHEP::cm/CLHEP::s;
const double holeV_sat_exp     = -0.52;
const double holeE_crit_0      = 1.24   * CLHEP::volt/CLHEP::cm;
const double holeE_crit_exp    = 1.68;
const double holeBeta_0        = 0.46;
const double holeBeta_exp      = 0.17;

const double temperatureZero   = 273.15 * CLHEP::kelvin;

FaserSiliconProperties::FaserSiliconProperties() 
  :  m_electronDriftMobility(0),
     m_holeDriftMobility(0),
     m_electronHallMobility(0),
     m_holeHallMobility(0),
     m_electronDiffusionConstant(0),
     m_holeDiffusionConstant(0),
     m_electronSaturationVelocity(0),
     m_holeSaturationVelocity(0),
     m_ehPairsPerEnergy(s_ehPairsPerEnergyDefault),
     m_override(false)
{}

FaserSiliconProperties::FaserSiliconProperties(double temperature, double electricField) 
  :  m_electronDriftMobility(0),
     m_holeDriftMobility(0),
     m_electronHallMobility(0),
     m_holeHallMobility(0),
     m_electronDiffusionConstant(0),
     m_holeDiffusionConstant(0),
     m_electronSaturationVelocity(0),
     m_holeSaturationVelocity(0),
     m_ehPairsPerEnergy(s_ehPairsPerEnergyDefault),
     m_override(false)
{
  setConditions(temperature, electricField);
}

void FaserSiliconProperties::setConditions(double temperature, double electricField) {

  if (m_electronSaturationVelocity==0.0) { m_electronSaturationVelocity=elecV_sat_0; }
  if (m_holeSaturationVelocity==0.0)     { m_holeSaturationVelocity=holeV_sat_0; }

  if (!m_override) {
    m_electronDriftMobility = calcElectronDriftMobility(temperature, electricField); 
    m_holeDriftMobility = calcHoleDriftMobility(temperature, electricField); 
    m_electronHallMobility = calcElectronHallFactor(temperature) * m_electronDriftMobility;
    m_holeHallMobility = calcHoleHallFactor(temperature) * m_holeDriftMobility;
    m_electronDiffusionConstant = calcDiffusionConstant(temperature, m_electronDriftMobility);  
    m_holeDiffusionConstant = calcDiffusionConstant(temperature, m_holeDriftMobility);  
  }
}

double FaserSiliconProperties::driftMobility(CarrierType carrierType) const {
  if (carrierType == holes) {
    return holeDriftMobility();
  } else {
    return electronDriftMobility();
  }
}

double FaserSiliconProperties::hallMobility(CarrierType carrierType) const {
  if (carrierType == holes) {
    return holeHallMobility();
  } else {
    return electronHallMobility();
  }
}

double FaserSiliconProperties::diffusionConstant(CarrierType carrierType) const {
  if (carrierType == holes) {
    return holeDiffusionConstant();
  } else {
    return electronDiffusionConstant();
  }
}

double FaserSiliconProperties::charge(CarrierType carrierType) const {
  return (carrierType == holes) ? +1 : -1;
}

double FaserSiliconProperties::signedHallMobility(CarrierType carrierType) const {
  if (carrierType == holes) {
    return holeHallMobility();
  } else {
    return -electronHallMobility();
  }
}

double FaserSiliconProperties::calcElectronHallFactor(double temperature) const {
  // Equation from ATL-INDET-2001-004
  return elecHallFactZero + elecHallFact_drdt * (temperature - temperatureZero);
}

double FaserSiliconProperties::calcHoleHallFactor(double temperature) const {
  // Equation from ATL-INDET-2001-004
  return holeHallFactZero + holeHallFact_drdt * (temperature - temperatureZero);
}

// driftMobility
double FaserSiliconProperties::calcDriftMobility(double electricField, double electricField_critical, double saturationVelocity, double beta) const {
  // Equation from ATL-INDET-2001-004
  return saturationVelocity / electricField_critical / 
    pow(std::abs(1. + pow(std::abs(electricField/electricField_critical), beta)), 1./beta);
}
  
double FaserSiliconProperties::calcElectronDriftMobility(double temperature, double electricField) const {
  // Equations from ATL-INDET-2001-004
//  double saturationVelocity = elecV_sat_0 * pow(temperature, elecV_sat_exp);
  double saturationVelocity = m_electronSaturationVelocity*pow(temperature, elecV_sat_exp);
  double electricField_critical = elecE_crit_0 * pow(temperature, elecE_crit_exp);
  double beta = elecBeta_0 * pow(temperature, elecBeta_exp);
  return calcDriftMobility(electricField, electricField_critical, saturationVelocity, beta);
} 

double FaserSiliconProperties::calcHoleDriftMobility(double temperature, double electricField) const {
  // Equations from ATL-INDET-2001-004
//  double saturationVelocity = holeV_sat_0 * pow(temperature, holeV_sat_exp);
  double saturationVelocity = m_holeSaturationVelocity*pow(temperature, holeV_sat_exp);
  double electricField_critical = holeE_crit_0 * pow(temperature, holeE_crit_exp);
  double beta = holeBeta_0 * pow(temperature, holeBeta_exp);
  return calcDriftMobility(electricField, electricField_critical, saturationVelocity, beta);
} 


double FaserSiliconProperties::calcDiffusionConstant(double temperature, double mobility) const {
  // Einstein's relationship (in many text books)
  return -CLHEP::k_Boltzmann * temperature / CLHEP::electron_charge * mobility; // CLHEP::k_Boltzmann and CLHEP::electron_charge
}

double FaserSiliconProperties::electronDriftMobility() const {
  return m_electronDriftMobility;
}

double FaserSiliconProperties::holeDriftMobility() const {
  return m_holeDriftMobility;
}

double FaserSiliconProperties::electronHallMobility() const {
  return m_electronHallMobility;
}

double FaserSiliconProperties::holeHallMobility() const {
  return m_holeHallMobility;
}

double FaserSiliconProperties::electronDiffusionConstant() const {
  return m_electronDiffusionConstant;
}

double FaserSiliconProperties::holeDiffusionConstant() const {
  return m_holeDiffusionConstant;
}

double FaserSiliconProperties::electronSaturationVelocity() const {
  return m_electronSaturationVelocity;
}

double FaserSiliconProperties::holeSaturationVelocity() const {
  return m_holeSaturationVelocity;
}

void FaserSiliconProperties::setElectronDriftMobility(double mobility) {
  m_override = true;
  m_electronDriftMobility = mobility;
}

void FaserSiliconProperties::setHoleDriftMobility(double mobility) {
  m_override = true;
  m_holeDriftMobility = mobility;
}

void FaserSiliconProperties::setElectronHallMobility(double mobility) {
  m_override = true;
  m_electronHallMobility = mobility;
}

void FaserSiliconProperties::setHoleHallMobility(double mobility) {
  m_override = true;
  m_holeHallMobility = mobility;
}

void FaserSiliconProperties::setElectronDiffusionConstant(double diffusionConstant) {
  m_override = true;
  m_electronDiffusionConstant = diffusionConstant;
}

void FaserSiliconProperties::setHoleDiffusionConstant(double diffusionConstant) {
  m_override = true;
  m_holeDiffusionConstant = diffusionConstant;
}

void FaserSiliconProperties::setElectronSaturationVelocity(double electronSaturationVelocity) {
  m_electronSaturationVelocity = electronSaturationVelocity;
}

void FaserSiliconProperties::setHoleSaturationVelocity(double holeSaturationVelocity) {
  m_holeSaturationVelocity = holeSaturationVelocity;
}

void FaserSiliconProperties::setElectronHolePairsPerEnergy(double ehPairsPerEnergy) {
  m_ehPairsPerEnergy = ehPairsPerEnergy; 
}

double FaserSiliconProperties::electronHolePairsPerEnergy() const {
   return m_ehPairsPerEnergy;
}



} // namespace Tracker
