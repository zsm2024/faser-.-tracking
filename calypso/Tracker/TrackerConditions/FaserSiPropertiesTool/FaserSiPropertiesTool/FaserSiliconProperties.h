/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSIPROPERTIESTOOL_FASERSILICONPROPERTIES_H
#define FASERSIPROPERTIESTOOL_FASERSILICONPROPERTIES_H

///////////////////////////////////////////////////////////////////
//   Header file for class SiliconProperties
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Class to hold silicon properties such as mobility, etc.
// Quantities are in CLHEP units.
///////////////////////////////////////////////////////////////////
// Authors: Grant Gorfine
///////////////////////////////////////////////////////////////////

#include "TrackerReadoutGeometry/TrackerDD_Defs.h"

namespace Tracker {

class FaserSiliconProperties
{
public:

  
  FaserSiliconProperties();
  FaserSiliconProperties(double temperature, double electricField);

  void setConditions(double temperature, double electricField);
  
  double electronDriftMobility() const;
  double holeDriftMobility() const;
  double electronHallMobility() const;
  double holeHallMobility() const;
  double electronDiffusionConstant() const;
  double holeDiffusionConstant() const;
  double electronSaturationVelocity() const;
  double holeSaturationVelocity() const;
  
  double electronHolePairsPerEnergy() const;
  
  double driftMobility(TrackerDD::CarrierType carrier) const;
  double hallMobility(TrackerDD::CarrierType carrier) const;
  double signedHallMobility(TrackerDD::CarrierType carrier) const; // signed by the charge of the carrier.
  double diffusionConstant(TrackerDD::CarrierType carrier) const;
  double charge(TrackerDD::CarrierType carrier) const;


  // These are mainly for use internally but are left public 
  double calcElectronHallFactor(double temperature) const;
  double calcHoleHallFactor(double temperature) const;
  double calcDriftMobility(double electricField, double electricField_critical, 
			   double saturationVelocity, double beta) const;
  double calcElectronDriftMobility(double temperature, double electricField) const;
  double calcHoleDriftMobility(double temperature, double electricField) const;
  double calcElectronHallMobility(double temperature, double mobility) const;
  double calcHoleHallMobility(double temperature, double mobility) const;
  double calcDiffusionConstant(double temperature, double mobility) const;

  // Allow overriding calculated quantities.
  // Setting any one (other than setElectronHolePairsPerEnergy) will disable recalculations 
  // ie setConditions(temperature, electricField) will have no further effect.
  void setElectronDriftMobility(double mobility);
  void setHoleDriftMobility(double mobility);
  void setElectronHallMobility(double mobility);
  void setHoleHallMobility(double mobility);
  void setElectronDiffusionConstant(double diffusionConstant);
  void setHoleDiffusionConstant(double diffusionConstant);
  void setElectronSaturationVelocity(double electronSaturationVelocity);
  void setHoleSaturationVelocity(double holeSaturationVelocity);
  void setElectronHolePairsPerEnergy(double ehPairsPerEnergy);

private:
  double m_electronDriftMobility;
  double m_holeDriftMobility;
  double m_electronHallMobility;
  double m_holeHallMobility;
  double m_electronDiffusionConstant;
  double m_holeDiffusionConstant;
  double m_electronSaturationVelocity;
  double m_holeSaturationVelocity;
  double m_ehPairsPerEnergy;

  bool   m_override; // signal that quantities are overriden and recalculation are disabled.

  const static double s_ehPairsPerEnergyDefault;
};

} // namespace Tracker

#endif // FASERSIPROPERTIESTOOL_FASERSILICONPROPERTIES_H
