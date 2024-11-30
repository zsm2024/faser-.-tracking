/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSiLorentzAngleTool.h"

#include <algorithm>
#include <limits>

#include "GaudiKernel/SystemOfUnits.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "Identifier/IdentifierHash.h"
#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "FaserSiPropertiesTool/FaserSiliconProperties.h"

FaserSiLorentzAngleTool::FaserSiLorentzAngleTool(const std::string& type, const std::string& name, const IInterface* parent):
  base_class(type, name, parent),
  m_condData{"SCTSiLorentzAngleCondData"},
  m_magFieldSvc{"FaserFieldSvc", name}
{
  declareProperty("IgnoreLocalPos", m_ignoreLocalPos = false, 
                  "Treat methods that take a local position as if one called the methods without a local position");
  // IF SCT, YOU NEED TO USE THE SAME PROPERTIES AS USED IN SCTSiLorentzAngleCondAlg!!!
  declareProperty("DetectorName", m_detectorName="SCT", "Detector name (SCT)");
  declareProperty("MagFieldSvc", m_magFieldSvc);
  declareProperty("NominalField", m_nominalField = 2.0834*Gaudi::Units::tesla);
  declareProperty("UseMagFieldSvc", m_useMagFieldSvc = true);
  declareProperty("SiLorentzAngleCondData", m_condData, "Key of input FaserSiLorentzAngleCondData");
}

StatusCode FaserSiLorentzAngleTool::initialize() { 

  ATH_MSG_DEBUG("FaserSiLorentzAngleTool Initialized");

  if (m_detectorName not_eq "SCT") {
    ATH_MSG_FATAL("Invalid detector name: " << m_detectorName << ". Must be SCT.");
    return StatusCode::FAILURE;
  }

  // Read Cond Handle
  ATH_CHECK(m_condData.initialize());
  ATH_CHECK(m_detEleCollKey.initialize());

  // MagneticFieldSvc handles updates itself
  if (not m_useMagFieldSvc) {
    ATH_MSG_DEBUG("Not using MagneticFieldSvc - Will be using Nominal Field!");
  } else if (m_magFieldSvc.retrieve().isFailure()) {
    ATH_MSG_WARNING("Could not retrieve MagneticFieldSvc - Will be using Nominal Field!");
    m_useMagFieldSvc = false;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode FaserSiLorentzAngleTool::finalize() {
  return StatusCode::SUCCESS;
}

double FaserSiLorentzAngleTool::getLorentzShift(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getLorentzShift(elementHash);
  }
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getLorentzShift(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const {
  if (m_ignoreLocalPos) return getLorentzShift(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, LorentzShift);
}

double FaserSiLorentzAngleTool::getLorentzShiftEta(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getLorentzShiftEta(elementHash);
  }
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getLorentzShiftEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const {
  if (m_ignoreLocalPos) return getLorentzShiftEta(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, LorentzShiftEta);
}

double FaserSiLorentzAngleTool::getTanLorentzAngle(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getTanLorentzAngle(elementHash);
  }
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getTanLorentzAngle(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const {
  if (m_ignoreLocalPos) return getTanLorentzAngle(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, TanLorentzAngle);
}

double FaserSiLorentzAngleTool::getTanLorentzAngleEta(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getTanLorentzAngleEta(elementHash);
  }
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getTanLorentzAngleEta(const IdentifierHash& elementHash, const Amg::Vector2D& locPos) const {
  if (m_ignoreLocalPos) return getTanLorentzAngleEta(elementHash);
  // The cache is used to store the results. The cache is therefore invalidated if we specify a position.
  return getValue(elementHash, locPos, TanLorentzAngleEta);
}

double FaserSiLorentzAngleTool::getBiasVoltage(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getBiasVoltage(elementHash);
  }
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getTemperature(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getTemperature(elementHash);
  }
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getDepletionVoltage(const IdentifierHash& elementHash) const {
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getDepletionVoltage(elementHash);
  }
  return s_invalidValue;
}
   
double FaserSiLorentzAngleTool::getValue(const IdentifierHash& elementHash, const Amg::Vector2D& locPos, Variable variable) const {
  if (not (variable==TanLorentzAngle or variable==LorentzShift or variable==TanLorentzAngleEta or variable==LorentzShiftEta)) {
    ATH_MSG_WARNING("getValue with Variable=" << variable << " is not available");
    return s_invalidValue;
  }

  double temperature{getTemperature(elementHash)};
  double deplVoltage{getDepletionVoltage(elementHash)};
  double biasVoltage{getBiasVoltage(elementHash)};

  // Calculate depletion depth. If biasVoltage is less than depletionVoltage
  // the detector is not fully depleted and we need to take this into account.
  // We take absolute values just in case voltages are signed.
  const TrackerDD::SiDetectorElement* element{getDetectorElement(elementHash)};
  double depletionDepth{element->thickness()};
  if (deplVoltage==0.0) ATH_MSG_WARNING("Depletion voltage in "<<__FILE__<<" is zero, which might be a bug.");
  if (std::abs(biasVoltage) < std::abs(deplVoltage)) {
    depletionDepth *= sqrt(std::abs(biasVoltage / deplVoltage));
  }
  double meanElectricField{0.};
  if (depletionDepth) { 
    meanElectricField = biasVoltage / depletionDepth;
  }
  double mobility{0.};
  Tracker::FaserSiliconProperties siProperties;
  siProperties.setConditions(temperature, meanElectricField);
  mobility = siProperties.signedHallMobility(element->carrierType());
  // Get magnetic field.
  Amg::Vector3D pointvec{element->globalPosition(locPos)};
  Amg::Vector3D magneticField{getMagneticField(pointvec)};

  double correctionFactor{getCorrectionFactor()};

  // The angles are in the hit frame. This is because that is what is needed by the digization and also
  // gives a more physical sign of the angle (ie dosen't flip sign when the detector is flipped).
  // The hit depth axis is pointing from the readout side to the backside if  m_design->readoutSide() < 0
  // The hit depth axis is pointing from the backside to the readout side if  m_design->readoutSide() > 0
  if (variable==TanLorentzAngle or variable==LorentzShift) {
    double tanLorentzAnglePhi{element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitPhiDirection()*(element->normal().cross(magneticField)).dot(element->phiAxis())};
    if (variable==TanLorentzAngle) {
      return correctionFactor*tanLorentzAnglePhi;
    }
    // This gives the effective correction in the reconstruction frame hence the extra hitPhiDirection()
    // as the angle above is in the hit frame.
    double lorentzCorrectionPhi{-0.5*element->hitPhiDirection()*tanLorentzAnglePhi*depletionDepth};
    return correctionFactor*lorentzCorrectionPhi;
  }

  // The Lorentz eta shift very small and so can be ignored, but we include it for completeness.
  // It is < ~0.1 um in the pixel.
  // In the SCT its largest in the stereo side of the barrel modules where it is about 0.3 micron along the strip. 
  if (variable==TanLorentzAngleEta or variable==LorentzShiftEta) {
    double tanLorentzAngleEta{element->design().readoutSide()*mobility*element->hitDepthDirection()*element->hitEtaDirection()*(element->normal().cross(magneticField)).dot(element->etaAxis())};
    if (variable==TanLorentzAngleEta) {
      return correctionFactor*tanLorentzAngleEta;
    }
    double lorentzCorrectionEta{-0.5*element->hitPhiDirection()*tanLorentzAngleEta*depletionDepth};
    return correctionFactor*lorentzCorrectionEta;
  }

  ATH_MSG_WARNING("You should not see this message. Something is wrong in getValue");
  return s_invalidValue;
}

double FaserSiLorentzAngleTool::getCorrectionFactor() const
{
  const FaserSiLorentzAngleCondData* condData{getCondData()};
  if (condData) {
    return condData->getCorrectionFactor();
  }
  return s_invalidValue;
}

Amg::Vector3D FaserSiLorentzAngleTool::getMagneticField(const Amg::Vector3D& pointvec) const {
  // Get the magnetic field.
  if (m_useMagFieldSvc) {
    ATH_MSG_VERBOSE("Getting magnetic field from magnetic field service.");
    double field[3];
    double point[3];
    point[0] = pointvec[0];
    point[1] = pointvec[1];
    point[2] = pointvec[2];
    m_magFieldSvc->getField(point, field);
    return Amg::Vector3D(field[0], field[1], field[2]);
  } else {
    ATH_MSG_VERBOSE("Using Nominal Field");
    return Amg::Vector3D(0., 0., m_nominalField);
  }
}

const FaserSiLorentzAngleCondData* FaserSiLorentzAngleTool::getCondData() const {
  SG::ReadCondHandle<FaserSiLorentzAngleCondData> handle{m_condData};
  if (handle.isValid()) {
    const FaserSiLorentzAngleCondData* data{*handle};
    return data;
  }
  ATH_MSG_WARNING(m_condData.key() << " cannot be retrieved.");
  return nullptr;
}

const TrackerDD::SiDetectorElement* FaserSiLorentzAngleTool::getDetectorElement(const IdentifierHash& waferHash) const {
  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> handle{m_detEleCollKey};
  const TrackerDD::SiDetectorElementCollection* elements{nullptr};
  if (handle.isValid()) elements = *handle;
  if (elements!=nullptr) return elements->getDetectorElement(waferHash);

  ATH_MSG_WARNING(m_detEleCollKey.key() << " cannot be retrieved.");
  return nullptr;
}

const double FaserSiLorentzAngleTool::s_invalidValue{std::numeric_limits<double>::quiet_NaN()};
