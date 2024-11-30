/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_FrontEnd.h"

// Random number
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussZiggurat.h"  // for RandGaussZiggurat
#include "CLHEP/Random/RandPoisson.h"
#include "CLHEP/Random/RandomEngine.h"

#include "FaserSiDigitization/SiHelper.h"
#include "FaserSCT_Digitization/ISCT_Amp.h"

#include "TrackerIdentifier/FaserSCT_ID.h"

#include "TrackerReadoutGeometry/SCT_DetectorManager.h"
#include "TrackerReadoutGeometry/SCT_ModuleSideDesign.h"

// C++ Standard Library
#include <algorithm>
#include <iostream>

// #define SCT_DIG_DEBUG

using namespace std;
using namespace TrackerDD;

// constructor
FaserSCT_FrontEnd::FaserSCT_FrontEnd(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent) {
}

// ----------------------------------------------------------------------
// Initialize
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::initialize() {
  if (m_NoiseOn and (not m_analogueNoiseOn)) {
    ATH_MSG_FATAL("AnalogueNoiseOn/m_analogueNoiseOn should be true if NoiseOn/m_NoiseOn is true.");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("FaserSCT_FrontEnd::initialize()");
  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_sct_id, "FaserSCT_ID"));
  // Get SCT detector manager
  ATH_CHECK(detStore()->retrieve(m_SCTdetMgr, "SCT"));
  // Get the amplifier tool
  ATH_CHECK(m_sct_amplifier.retrieve());
  ATH_MSG_DEBUG("SCT Amplifier tool located ");

  // Get the SCT_ReadCaliDataSvc
  if (m_useCalibData) {
    ATH_CHECK(m_ReadCalibChipDataTool.retrieve());
    ATH_MSG_DEBUG("CalibChipData Service located ");
    // ATH_MSG_FATAL("Use of calibration data not supported.");
  } else {
    m_ReadCalibChipDataTool.disable();
  }

  // Get the maximum number of strips of any module
  m_strip_max = m_SCTdetMgr->numerology().maxNumStrips();

  constexpr float fC = 6242.2;
  m_Threshold = m_Threshold * fC;

#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("\tNoise factors:");
  ATH_MSG_INFO("\tBarrel = " << m_NoiseBarrel << " Outer Barrel = " << m_NoiseBarrel3 <<
               " EC, inners = " << m_NoiseInners << " EC, middles = " << m_NoiseMiddles <<
               " EC, short middles = " << m_NoiseShortMiddles << " EC, outers = " << m_NoiseOuters);
  ATH_MSG_INFO("\tThreshold=" << m_Threshold << " fC, time of Threshold=" << m_timeOfThreshold);
#endif

  ATH_MSG_INFO("m_Threshold             (Threshold)           = " << m_Threshold);
  ATH_MSG_INFO("m_timeOfThreshold       (TimeOfThreshold)     = " << m_timeOfThreshold);
  ATH_MSG_INFO("m_data_compression_mode (DataCompressionMode) = " << m_data_compression_mode);
  ATH_MSG_INFO("m_data_readout_mode     (DataReadOutMode)     = " <<  m_data_readout_mode);

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_FrontEnd::finalize() {
#ifdef SCT_DIG_DEBUG
  ATH_MSG_INFO("FaserSCT_FrontEnd::finalize()");
#endif
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// Init the class variable  vectors
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::initVectors(int strips, SCT_FrontEndData& data) const {
  data.m_Offset.assign(strips, 0.0);
  data.m_GainFactor.assign(strips, 0.0);
  data.m_NoiseFactor.assign(strips, 0.0);

  if (m_data_compression_mode == 1 and m_data_readout_mode == 0) {
    data.m_Analogue[0].reserve(1);
    data.m_Analogue[1].reserve(strips);
  } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) {
    data.m_Analogue[0].reserve(strips);
    data.m_Analogue[1].reserve(strips);
  } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) {
    data.m_Analogue[0].reserve(strips);
    data.m_Analogue[1].reserve(strips);
    data.m_Analogue[2].reserve(strips);
  } else {
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// prepare gain and offset for the strips for a given module
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::prepareGainAndOffset(SiChargedDiodeCollection& collection, /*const Identifier& moduleId,*/ CLHEP::HepRandomEngine * rndmEngine, SCT_FrontEndData& data) const {
  // now we need to generate gain and offset channel by channel: some algebra
  // for generation of partially correlated random numbers
  float W = m_OGcorr * m_GainRMS * m_Ospread / (m_GainRMS * m_GainRMS - m_Ospread * m_Ospread);
  float A = 4 * W * W + 1.0;
  float x1 = (A - sqrt(A)) / (2.0 * A);
  float sinfi = sqrt(x1);
  float cosfi = sqrt(1.0 - x1);

  sinfi = sinfi * m_OGcorr / fabs(m_OGcorr);
  float S = m_GainRMS * m_GainRMS + m_Ospread * m_Ospread;
  float D = (m_GainRMS * m_GainRMS - m_Ospread * m_Ospread) / (cosfi * cosfi - sinfi * sinfi);
  float S1 = sqrt((S + D) * 0.5);
  float S2 = sqrt((S - D) * 0.5);
  float Noise = 0;
  int mode = 1;

  // To set noise values for different module types, barrel, EC, inners, middles, short middles, and outers
  if (m_analogueNoiseOn) {
    // if (m_sct_id->barrel_ec(moduleId) == 0) { // barrel_ec=0 corresponds to barrel
    //   if (m_sct_id->layer_disk(moduleId) == 3) { // outer barrel layer 10 degrees warmer
        Noise = m_NoiseBarrel3;
    //   } else {
    //     Noise = m_NoiseBarrel;
    //   }
    // } else {
    //   int moduleType = m_sct_id->eta_module(moduleId);
    //   switch (moduleType) { // eta = 0, 1, or 2 corresponds to outers, middles and inners?! (at least in the offline world)
    //   case 0: {
    //     Noise = m_NoiseOuters;
    //     break;
    //   }
    //   case 1: {
    //     if (m_sct_id->layer_disk(moduleId) == 7) {
    //       Noise = m_NoiseShortMiddles;
    //     } else {
    //       Noise = m_NoiseMiddles;
    //     }
    //     break;
    //   }
    //   case 2: {
    //     Noise = m_NoiseInners;
    //     break;
    //   }
    //   default: {
    //     Noise = m_NoiseBarrel;
    //     ATH_MSG_ERROR("moduleType(eta): " << moduleType << " unknown, using barrel");
    //   }
    //   }// end of switch structure
    // }
  }

  // Loop over collection and setup gain/offset/noise for the hit and neighbouring strips
  SiChargedDiodeIterator i_chargedDiode = collection.begin();
  SiChargedDiodeIterator i_chargedDiode_end = collection.end();

  for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
    SiChargedDiode diode = (*i_chargedDiode).second;
    // should be const as we aren't trying to change it here - but getReadoutCell() is not a const method...
    unsigned int flagmask = diode.flag() & 0xFE;
    // Get the flag for this diode ( if flagmask = 1 If diode is disconnected/disabled skip it)
    if (!flagmask) { // If the diode is OK (not flagged)
      const SiReadoutCellId roCell = diode.getReadoutCell();
      if (roCell.isValid()) {
        int strip = roCell.strip();
        int i = std::max(strip - 1, 0);
        int i_end = std::min(strip + 2, m_strip_max.load());

        // loop over strips
        for (; i < i_end; i++) {
          // Need to check if strip is already setup
          if (data.m_Analogue[1][i] <= 0.0) {
            float g = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0.0, S1);
            float o = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0.0, S2);

            data.m_GainFactor[i] = 1.0 + (cosfi * g + sinfi * o);
            data.m_Offset[i] = (cosfi * o - sinfi * g);
            data.m_NoiseFactor[i] = Noise * mode;

            // Fill the noise and offset values into the Analogue
            if (m_data_compression_mode == 1 and m_data_readout_mode == 0) { // level mode x1x
              data.m_Analogue[1][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
            } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) { // edge mode 01x
              data.m_Analogue[0][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
              data.m_Analogue[1][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
            } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) { // any hit mode xxx or expanded read out mode
              data.m_Analogue[0][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
              data.m_Analogue[1][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
              data.m_Analogue[2][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// prepare gain and offset for the strips for a given module using
// Cond Db data to get the chip calibration data
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::prepareGainAndOffset(SiChargedDiodeCollection& collection, int side, const Identifier& moduleId, CLHEP::HepRandomEngine * rndmEngine, SCT_FrontEndData& data) const {
  // Get chip data from calib DB
  std::vector<float> gainByChipVect = m_ReadCalibChipDataTool->getNPtGainData(moduleId, side, "GainByChip");
  std::vector<float> gainRMSByChipVect = m_ReadCalibChipDataTool->getNPtGainData(moduleId, side, "GainRMSByChip");
  std::vector<float> offsetByChipVect = m_ReadCalibChipDataTool->getNPtGainData(moduleId, side, "OffsetByChip");
  std::vector<float> offsetRMSByChipVect = m_ReadCalibChipDataTool->getNPtGainData(moduleId, side, "OffsetRMSByChip");
  std::vector<float> noiseByChipVect(6, 0.0);

  if (m_analogueNoiseOn) { // Check if noise should be on or off
    noiseByChipVect = m_ReadCalibChipDataTool->getNPtGainData(moduleId, side, "NoiseByChip");
  }

  // Need to check if empty, most should have data, but a few old DEAD modules don't
  if (gainByChipVect.empty() or noiseByChipVect.empty()) {
    ATH_MSG_DEBUG("No calibration data in cond DB for module " << moduleId << " using JO values");
    if (StatusCode::SUCCESS != prepareGainAndOffset(collection, /*moduleId,*/ rndmEngine, data)) {
      return StatusCode::FAILURE;
    } else {
      return StatusCode::SUCCESS;
    }
  }

  // Don't really need to set up values for each chip...
  float gainMeanValue = meanValue(gainByChipVect);
  if (gainMeanValue < 0.0) {
    ATH_MSG_DEBUG("All chip gain values are 0 for module " << moduleId << " using JO values");
    if (StatusCode::SUCCESS != prepareGainAndOffset(collection, /*moduleId,*/ rndmEngine, data)) {
      return StatusCode::FAILURE;
    } else {
      return StatusCode::SUCCESS;
    }
  }

  std::vector<float> gain(6, 0.0);
  std::vector<float> offset(6, 0.0);
  std::vector<float> S1(6, 0.0);
  std::vector<float> S2(6, 0.0);
  std::vector<float> sinfi(6, 0.0);
  std::vector<float> cosfi(6, 0.0);
  float gainRMS = 0.0;
  float offsetRMS = 0.0;

  for (int i = 0; i < 6; ++i) {
    // Some very few chips have 0 values, dead/bypassed/etc, so check and use some fixed values instead
    if (gainByChipVect[i] > 0.1) {
      gain[i] = gainByChipVect[i] / gainMeanValue;
      offset[i] = offsetByChipVect[i] / m_Threshold;
      gainRMS = gainRMSByChipVect[i] / gainMeanValue;
      offsetRMS = offsetRMSByChipVect[i] / m_Threshold;
    } else {
      gain[i] = 55.0 / gainMeanValue;
      offset[i] = 42.0 / m_Threshold;
      gainRMS = 1.3 / gainMeanValue;
      offsetRMS = 2.0 / m_Threshold;
    }

    float W = m_OGcorr * gainRMS * offsetRMS / (gainRMS * gainRMS - offsetRMS * offsetRMS);
    float A = 4 * W * W + 1.0;
    float x1 = (A - sqrt(A)) / (2.0 * A);
    sinfi[i] = sqrt(x1);
    cosfi[i] = sqrt(1.0 - x1);
    sinfi[i] = sinfi[i] * m_OGcorr / fabs(m_OGcorr);
    float S = gainRMS * gainRMS + offsetRMS * offsetRMS;
    float D = (gainRMS * gainRMS - offsetRMS * offsetRMS) / (cosfi[i] * cosfi[i] - sinfi[i] * sinfi[i]);
    S1[i] = sqrt((S + D) / 2.0);
    S2[i] = sqrt((S - D) / 2.0);
  }

  // Loop over collection and setup gain/offset/noise for the hit and neighbouring strips
  SiChargedDiodeIterator i_chargedDiode = collection.begin();
  SiChargedDiodeIterator i_chargedDiode_end = collection.end();

  for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
    SiChargedDiode diode = (*i_chargedDiode).second;
    // should be const as we aren't trying to change it here - but getReadoutCell() is not a const method...
    unsigned int flagmask = diode.flag() & 0xFE;
    // Get the flag for this diode ( if flagmask = 1 If diode is disconnected/disabled skip it)
    if (!flagmask) { // If the diode is OK (not flagged)
      const SiReadoutCellId roCell = diode.getReadoutCell();

      if (roCell.isValid()) {
        int strip = roCell.strip();
        int i = std::max(strip - 1, 0);
        int i_end = std::min(strip + 2, m_strip_max.load());

        // loop over strips
        for (; i < i_end; i++) {
          // Need to check if strip is already setup
          if (data.m_Analogue[1][i] <= 0.0) {
            // Values depends on which chip the strip is on (complex when strip is on chip edge)
            int chip = i / 128;
            float g = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0.0, S1[chip]);
            float o = CLHEP::RandGaussZiggurat::shoot(rndmEngine, 0.0, S2[chip]);

            data.m_GainFactor[i] = gain[chip] + (cosfi[chip] * g + sinfi[chip] * o);
            data.m_Offset[i] = offset[chip]   + (cosfi[chip] * o - sinfi[chip] * g);
            data.m_NoiseFactor[i] = noiseByChipVect[chip];

            // Fill the noise and offset values into the Analogue
            if (m_data_compression_mode == 1 and m_data_readout_mode == 0) { // level mode x1x
              data.m_Analogue[1][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
            } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) { // edge mode 01x
              data.m_Analogue[0][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
              data.m_Analogue[1][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
            } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) { // any hit mode xxx or expanded read out mode
              data.m_Analogue[0][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
              data.m_Analogue[1][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
              data.m_Analogue[2][i] = data.m_Offset[i] + data.m_NoiseFactor[i] * CLHEP::RandGaussZiggurat::shoot(rndmEngine);
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::randomNoise(SiChargedDiodeCollection& collection, /*const Identifier& moduleId,*/ CLHEP::HepRandomEngine * rndmEngine, SCT_FrontEndData& data) const {
  // Add random noise

  double occupancy = 0.0;
  double NoiseOccupancy = 0.0;
  float Noise = 0.0;
  int nNoisyStrips = 0;
  double mode = 1.;

  const bool noise_expanded_mode = (m_data_compression_mode == 3 and m_data_readout_mode == 1);

  // Will give 3 times as much noise occupancy if running in any hit expanded mode
  if (noise_expanded_mode) {
    mode = 3.;
  }

  // Sets fixed noise occupancy values for different module types, barrel, EC,
  // inners, middles
  // short middles, and outers
//   if (m_sct_id->barrel_ec(moduleId) == 0) { // barrel_ec=0 corresponds to barrel
//     if (m_sct_id->layer_disk(moduleId) == 3) { // outer barrel layer 10 degrees warmer
      NoiseOccupancy = m_NOBarrel3;
      Noise = m_NoiseBarrel3;
//     } else {
//       NoiseOccupancy = m_NOBarrel;
//       Noise = m_NoiseBarrel;
//     }
//   } else {
//     int moduleType = m_sct_id->eta_module(moduleId);
//     switch (moduleType) { // eta = 0, 1, or 2 corresponds to outers, middles and inners?! (at least in the offline world)
//     case 0: {
//       NoiseOccupancy = m_NOOuters;
//       Noise = m_NoiseOuters;
//       break;
//     }
//     case 1: {
//       if (m_sct_id->layer_disk(moduleId) == 7) {
//         NoiseOccupancy = m_NOShortMiddles;
//         Noise = m_NoiseShortMiddles;
//       } else {
//         NoiseOccupancy = m_NOMiddles;
//         Noise = m_NoiseMiddles;
//       }
//       break;
//     }
//     case 2: {
//       NoiseOccupancy = m_NOInners;
//       Noise = m_NoiseInners;
//       break;
//     }
//     default: {
//       NoiseOccupancy = m_NOBarrel;
//       Noise = m_NoiseBarrel;
//       ATH_MSG_ERROR("moduleType(eta): " << moduleType << " unknown, using barrel");
//     }
//     }// end of switch structure
//   }

  // Calculate the number of "free strips"
  int nEmptyStrips = 0;
  std::vector<int> emptyStrips;
  emptyStrips.reserve(m_strip_max);
  for (int i = 0; i < m_strip_max; i++) {
    if (data.m_StripHitsOnWafer[i] == 0) {
      emptyStrips.push_back(i);
      ++nEmptyStrips;
    }
  }

  if (nEmptyStrips != 0) {
    // Should randomize the fixed NO values, so we get some differences per
    // wafer
    occupancy = CLHEP::RandGaussZiggurat::shoot(rndmEngine, NoiseOccupancy, NoiseOccupancy * 0.1);

    // Modify the occupancy if threshold is not 1.0 fC
    if (m_Threshold > 6242.3 or m_Threshold < 6242.1) {
      const float fC = 6242.2;
      occupancy = occupancy * exp(-(0.5 / (Noise * Noise) * (m_Threshold * m_Threshold - fC * fC)));
    }
    nNoisyStrips = CLHEP::RandPoisson::shoot(rndmEngine, m_strip_max * occupancy * mode);

    // Check and adapt the number of noisy strips to the number of free strips
    if (nEmptyStrips < nNoisyStrips) {
      nNoisyStrips = nEmptyStrips;
    }

    // Find random strips to get noise hits
    for (int i = 0; i < nNoisyStrips; i++) {
      int index = CLHEP::RandFlat::shootInt(rndmEngine, nEmptyStrips - i); // strip == 10, 12 free strips
      // have vector [10000100100200211001] 20 strips
      int strip = emptyStrips.at(index);
      emptyStrips.erase(emptyStrips.begin()+index); // Erase it not to use it again
      if (data.m_StripHitsOnWafer[strip]!=0) {
        ATH_MSG_ERROR(index << "-th empty strip, strip " << strip << " should be empty but is not empty! Something is wrong!");
      }
      data.m_StripHitsOnWafer[strip] = 3; // !< Random Noise hit
      // Add tbin info to noise diode
      if (noise_expanded_mode) { // !< if any hit mode, any time bin otherwise fixed tbin=2
        int noise_tbin = CLHEP::RandFlat::shootInt(rndmEngine, 3);
        // !< random number 0, 1 or 2
        if (noise_tbin == 0) {
          noise_tbin = 4; // !< now 1,2 or 4
        }
        if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, noise_tbin)) {
          ATH_MSG_ERROR("Can't add noise hit diode to collection");
        }
      } else {
        if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, 2)) {
          ATH_MSG_ERROR("Can't add noise hit diode to collection");
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::randomNoise(SiChargedDiodeCollection& collection, const Identifier& moduleId, int side, CLHEP::HepRandomEngine * rndmEngine, SCT_FrontEndData& data) const {
  const int n_chips = 6;
  const int chipStripmax = m_strip_max / n_chips;
  std::vector<float> NOByChipVect(n_chips, 0.0);
  std::vector<float> ENCByChipVect(n_chips, 0.0);
  std::vector<int> nNoisyStrips(n_chips, 0);
  double mode = 1.;

  const bool noise_expanded_mode = (m_data_compression_mode == 3 and m_data_readout_mode == 1);

  // Will give 3 times as much noise occupancy if running in any hit expanded mode
  if (noise_expanded_mode) {
    mode = 3.;
  }

  // Get chip data from calib DB
  NOByChipVect = m_ReadCalibChipDataTool->getNoiseOccupancyData(moduleId, side, "OccupancyByChip");
  ENCByChipVect = m_ReadCalibChipDataTool->getNPtGainData(moduleId, side, "NoiseByChip");

  // Need to check if empty, most should have data, but a few old DEAD modules don't, and 9C...
  if (NOByChipVect.empty()) {
    ATH_MSG_DEBUG("No calibration data in cond DB for module " << moduleId << " using JO values");
    if (StatusCode::SUCCESS != randomNoise(collection, /*moduleId,*/ rndmEngine, data)) {
      return StatusCode::FAILURE;
    } else {
      return StatusCode::SUCCESS;
    }
  } else {
    for (int i = 0; i < n_chips; i++) {
      // A 0 value can mean two things now, chip out of config for long time and no value was uploaded
      // or its short middles and inners and the value is for all purposes 0! so ok.

      // Modify the occupancy if threshold is not 1.0 fC
      if (m_Threshold > 6242.3 or m_Threshold < 6242.1) {
        constexpr float fC = 6242.2;
        NOByChipVect[i] = NOByChipVect[i] * exp(-(0.5 / (ENCByChipVect[i]*ENCByChipVect[i]) * (m_Threshold*m_Threshold - fC*fC)));
      }

      nNoisyStrips[i] = CLHEP::RandPoisson::shoot(rndmEngine, chipStripmax * NOByChipVect[i] * mode);
    }
  }

  // Loop over the chips on the wafer
  for (int chip_index = 0; chip_index < n_chips; ++chip_index) {
    int chip_strip_offset = chipStripmax * chip_index; // First strip number on chip

    // Calculate the number of "free strips" on this chip
    int nEmptyStripsOnChip = 0;
    std::vector<int> emptyStripsOnChip;
    emptyStripsOnChip.reserve(chipStripmax);
    for (int i = 0; i < chipStripmax; i++) {
      if (data.m_StripHitsOnWafer[i + chip_strip_offset] == 0) {
        emptyStripsOnChip.push_back(i);
        ++nEmptyStripsOnChip;
      }
    }

    // if no empty strips on chip do nothing
    if (nEmptyStripsOnChip != 0) {
      // Check and adapt the number of noisy strips to the number of free strips
      if (nEmptyStripsOnChip < nNoisyStrips[chip_index]) {
        nNoisyStrips[chip_index] = nEmptyStripsOnChip;
      }

      // Find random strips to get noise hits
      for (int i = 0; i < nNoisyStrips[chip_index]; i++) {
        int index = CLHEP::RandFlat::shootInt(rndmEngine, nEmptyStripsOnChip - i);
        int strip_on_chip = emptyStripsOnChip.at(index);
        emptyStripsOnChip.erase(emptyStripsOnChip.begin()+index); // Erase it not to use it again
        int strip = strip_on_chip + chip_strip_offset;
        if (data.m_StripHitsOnWafer[strip]!=0) {
          ATH_MSG_ERROR(index << "-th empty strip, strip " << strip << " should be empty but is not empty! Something is wrong!");
        }
        data.m_StripHitsOnWafer[strip] = 3; // !< Random Noise hit
        // Add tbin info to noise diode
        if (noise_expanded_mode) { // !< if any hit mode, any time bin
          // !< otherwise fixed tbin=2
          int noise_tbin = CLHEP::RandFlat::shootInt(rndmEngine, 3);
          // !< random number 0, 1 or 2
          if (noise_tbin == 0) {
            noise_tbin = 4; // !< now 1, 2 or 4
          }
          if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, noise_tbin)) {
            ATH_MSG_ERROR("Can't add noise hit diode to collection");
          }
        } else {
          if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, 2)) {
            ATH_MSG_ERROR("Can't add noise hit diode to collection");
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
// process the collection of pre digits this will need to go through
// all single-strip pre-digits calculate the amplifier response add noise
// (this could be moved elsewhere later) apply threshold do clustering
// ----------------------------------------------------------------------
void FaserSCT_FrontEnd::process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const {
  // get SCT module side design and check it
  const SCT_ModuleSideDesign *p_design = dynamic_cast<const SCT_ModuleSideDesign*>(&(collection.design()));

  if (p_design==nullptr) {
    return;
  }

  SCT_FrontEndData data;

  // Check number of strips in design and from manager(max number of strips on any module)
  // The design value should always be equal or lower than the manager one
  // However, no resising is now done in case of a lower value
  const int strip_max = p_design->cells();
  // Init vectors
  if (StatusCode::SUCCESS != initVectors(strip_max, data)) {
    ATH_MSG_ERROR("Can't resize class variable vectors");
    return;
  }

  // Contains strip hit info, reset to 0 for each wafer processed
  data.m_StripHitsOnWafer.assign(m_strip_max, 0);

  // Containes the charge for each bin on each hit strip
  if (m_data_compression_mode == 1 and m_data_readout_mode == 0) {
    for (int i = 0; i < m_strip_max; ++i) {
      data.m_Analogue[1][i] = 0.0;
    }
  } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) {
    for (int i = 0; i < m_strip_max; ++i) {
      data.m_Analogue[0][i] = 0.0;
      data.m_Analogue[1][i] = 0.0;
    }
  } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) {
    for (int i = 0; i < m_strip_max; ++i) {
      data.m_Analogue[0][i] = 0.0;
      data.m_Analogue[1][i] = 0.0;
      data.m_Analogue[2][i] = 0.0;
    }
  }

  // Get wafer, moduleId and side
  Identifier waferId = collection.identify();
  Identifier moduleId = m_sct_id->module_id(waferId);
  const int side = m_sct_id->side(waferId);

  // Check if collection empty
  if (not collection.empty()) {
    // Setup gain/offset/noise to the hit and neighbouring strips
    if (m_useCalibData) { // Use calib cond DB data
        // ATH_MSG_FATAL("Use of calibration data not supported.");
      if (StatusCode::SUCCESS != prepareGainAndOffset(collection, side, moduleId, rndmEngine, data)) {
        ATH_MSG_ERROR("\tCan't prepare Gain and Offset");
      }
    } else { // Use JO values
      if (StatusCode::SUCCESS != prepareGainAndOffset(collection, /*moduleId,*/ rndmEngine, data)) {
        ATH_MSG_ERROR("\tCan't prepare Gain and Offset");
      }
    }

    if (StatusCode::SUCCESS != doSignalChargeForHits(collection, data)) {
      ATH_MSG_ERROR("\tCan't doSignalChargeForHits");
    }

    if (StatusCode::SUCCESS != doThresholdCheckForRealHits(collection, data)) {
      ATH_MSG_ERROR("\tCan't doThresholdCheckForRealHits");
    }

    if (StatusCode::SUCCESS != doThresholdCheckForCrosstalkHits(collection, data)) {
      ATH_MSG_ERROR("\tCan't doThresholdCheckForCrosstalkHits");
    }
  }

  if (m_NoiseOn) {
    if (m_useCalibData) { // Check if using DB or not
        // ATH_MSG_FATAL("Use of calibration data not supported.");
      if (StatusCode::SUCCESS != randomNoise(collection, moduleId, side, rndmEngine, data)) {
        ATH_MSG_ERROR("\tCan't do random noise on wafer?!");
      }
    } else { // Use JO fixed values
      if (StatusCode::SUCCESS != randomNoise(collection, /*moduleId,*/ rndmEngine, data)) {
        ATH_MSG_ERROR("\tCan't do random noise on wafer?!");
      }
    }
  }

  // Check for strips above threshold and do clustering
  if (StatusCode::SUCCESS != doClustering(collection, data)) {
    ATH_MSG_ERROR("\tCan't cluster the hits?!");
  }
}

StatusCode FaserSCT_FrontEnd::doSignalChargeForHits(SiChargedDiodeCollection& collection, SCT_FrontEndData& data) const {
  typedef SiTotalCharge::list_t list_t;

  // *****************************************************************************
  // Loop over the diodes (strips ) and for each of them define the total signal
  // *****************************************************************************

  // set up number of needed bins depending on the compression mode
  short bin_max = 0;
  if (m_data_readout_mode == 0) {
    bin_max = m_data_compression_mode;
  } else {
    bin_max = 3;
  }

  std::vector<float> response(bin_max);

  SiChargedDiodeIterator i_chargedDiode = collection.begin();
  SiChargedDiodeIterator i_chargedDiode_end = collection.end();
  for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
    SiChargedDiode diode = (*i_chargedDiode).second;
    // should be const as we aren't trying to change it here - but getReadoutCell() is not a const method...
    unsigned int flagmask = diode.flag() & 0xFE;
    // Get the flag for this diode ( if flagmask = 1 If diode is disconnected/disabled skip it)
    if (!flagmask) { // If the diode is OK (not flagged)
      const SiReadoutCellId roCell = diode.getReadoutCell();

      if (roCell.isValid()) {
        int strip = roCell.strip();

        const list_t &ChargesOnStrip = diode.totalCharge().chargeComposition();

        if (m_data_compression_mode == 1 and m_data_readout_mode == 0) { // level mode x1x
          // Amplifier response
          data.m_Analogue[1][strip] += data.m_GainFactor[strip] * m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold);

          // Add Crosstalk signal for neighboring strip
          response[0] = m_sct_amplifier->crosstalk(ChargesOnStrip, m_timeOfThreshold);
          if (strip + 1 < m_strip_max) {
            data.m_Analogue[1][strip + 1] += data.m_GainFactor[strip + 1] * response[0];
          }
          if (strip > 0) {
            data.m_Analogue[1][strip - 1] += data.m_GainFactor[strip - 1] * response[0];
          }
        } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) { // edge mode 01x
          // Amplifier response
          m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold, response);
          for (short bin = 0; bin < bin_max; ++bin) {
            data.m_Analogue[bin][strip] += data.m_GainFactor[strip] * response[bin];
          }
          // Add Crosstalk signal for neighboring strip
          m_sct_amplifier->crosstalk(ChargesOnStrip, m_timeOfThreshold, response);
          for (short bin = 0; bin < bin_max; ++bin) {
            if (strip + 1 < m_strip_max) {
              data.m_Analogue[bin][strip + 1] += data.m_GainFactor[strip + 1] * response[bin];
            }
            if (strip > 0) {
              data.m_Analogue[bin][strip - 1] += data.m_GainFactor[strip - 1] * response[bin];
            }
          }
        } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) { // any hit mode xxx or expanded read out mode
          // Amplifier response
          m_sct_amplifier->response(ChargesOnStrip, m_timeOfThreshold, response);
          for (short bin = 0; bin < bin_max; ++bin) {
            data.m_Analogue[bin][strip] += data.m_GainFactor[strip] * response[bin];
          }
          // Add Crosstalk signal for neighboring strip
          m_sct_amplifier->crosstalk(ChargesOnStrip, m_timeOfThreshold, response);
          for (short bin = 0; bin < bin_max; ++bin) {
            if (strip + 1 < m_strip_max) {
              data.m_Analogue[bin][strip + 1] += data.m_GainFactor[strip + 1] * response[bin];
            }
            if (strip > 0) {
              data.m_Analogue[bin][strip - 1] += data.m_GainFactor[strip - 1] * response[bin];
            }
          }
        }
      } else { // if roCell not valid
        ATH_MSG_WARNING("\t Cannot get the cell ");
      }
    } else {// If diode is disconnected/disabled skip it
      ATH_MSG_WARNING("\tDisabled or disconnected diode (strip)");
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_FrontEnd::doThresholdCheckForRealHits(SiChargedDiodeCollection& collection, SCT_FrontEndData& data) const {
  // **********************************************************************************
  // Flag strips below threshold and flag the threshold check into data.m_StripHitsOnWafer
  // **********************************************************************************

  SiChargedDiodeIterator i_chargedDiode = collection.begin();
  SiChargedDiodeIterator i_chargedDiode_end = collection.end();

  for (; i_chargedDiode != i_chargedDiode_end; ++i_chargedDiode) {
    SiChargedDiode& diode = (*i_chargedDiode).second;
    SiReadoutCellId roCell = diode.getReadoutCell();
    if (roCell.isValid()) {
      int strip = roCell.strip();
      if (strip > -1 and strip < m_strip_max) {
        if (m_data_compression_mode == 1 and m_data_readout_mode == 0) { // level mode x1x
          if (data.m_Analogue[1][strip] < m_Threshold) {
            SiHelper::belowThreshold(diode, true);   // Below strip diode signal threshold
            data.m_StripHitsOnWafer[strip] = -1;
          } else if (((0x10 & diode.flag()) == 0x10) or ((0x4 & diode.flag()) == 0x4)) {
            // previously a crazy strip number could have screwed things up here.
            data.m_StripHitsOnWafer[strip] = -1;
          } else {
            data.m_StripHitsOnWafer[strip] = 1;
            SiHelper::SetTimeBin(diode, 2, &msg()); // set timebin info
          }
        } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) { // edge mode 01x
          if ((data.m_Analogue[0][strip] >= m_Threshold or data.m_Analogue[1][strip] < m_Threshold)) {
            SiHelper::belowThreshold(diode, true);   // Below strip diode signal threshold
            data.m_StripHitsOnWafer[strip] = -1;
          } else if (((0x10 & diode.flag()) == 0x10) or ((0x4 & diode.flag()) == 0x4)) {
            // previously a crazy strip number could have screwed things up here.
            data.m_StripHitsOnWafer[strip] = -1;
          } else {
            data.m_StripHitsOnWafer[strip] = 1;
            SiHelper::SetTimeBin(diode, 2, &msg()); // set timebin info
          }
        } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) { // Check hit pattern
          int have_hit_bin = 0;
          if (data.m_Analogue[0][strip] >= m_Threshold) {
            have_hit_bin = 4;
          }
          if (data.m_Analogue[1][strip] >= m_Threshold) {
            have_hit_bin += 2;
          }
          if (data.m_Analogue[2][strip] >= m_Threshold) {
            have_hit_bin += 1;
          }
          if (((0x10 & diode.flag()) == 0x10) || ((0x4 & diode.flag()) == 0x4)) {
            // previously a crazy strip number could have screwed things up here.
            data.m_StripHitsOnWafer[strip] = -1;
          } else if (m_data_compression_mode == 1) { // !< level and expanded mode
            if (have_hit_bin == 2 or have_hit_bin == 3 or have_hit_bin == 6 or have_hit_bin == 7) {
              data.m_StripHitsOnWafer[strip] = 1;
              SiHelper::SetTimeBin(diode, have_hit_bin, &msg());
            } else {
              SiHelper::belowThreshold(diode, true); // Below strip diode signal threshold
              data.m_StripHitsOnWafer[strip] = -1;
            }
          } else if (m_data_compression_mode == 2) { // !< edge and expanded mode
            if (have_hit_bin == 2 or have_hit_bin == 3) {
              data.m_StripHitsOnWafer[strip] = 1;
              SiHelper::SetTimeBin(diode, have_hit_bin, &msg());
            } else {
              SiHelper::belowThreshold(diode, true); // Below strip diode signal threshold
              data.m_StripHitsOnWafer[strip] = -1;
            }
          } else if (m_data_compression_mode == 3) { // !< any hit mode
            if (have_hit_bin == 0) {
              SiHelper::belowThreshold(diode, true); // Below strip diode signal threshold
              data.m_StripHitsOnWafer[strip] = -1;
            } else {
              data.m_StripHitsOnWafer[strip] = 1;
              if (m_data_readout_mode == 1) { // !< check for exp mode or not
                SiHelper::SetTimeBin(diode, have_hit_bin, &msg());
              } else {
                SiHelper::SetTimeBin(diode, 2, &msg());
              }
            }
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

// ----------------------------------------------------------------------
//
// ----------------------------------------------------------------------
StatusCode FaserSCT_FrontEnd::doThresholdCheckForCrosstalkHits(SiChargedDiodeCollection& collection, SCT_FrontEndData& data) const {
  // Check for noise+crosstalk strips above threshold
  // data.m_StripHitsOnWafer: real hits above threshold == 1 or below/disconnected
  // == -1
  // =0 for free strips or strips with charge to be checked (data.m_Analogue[1]!=0)
  // Set 2 for crosstalk noise hits and -2 for below ones

  for (int strip = 0; strip < m_strip_max; strip++) {
    // Find strips with data.m_StripHitsOnWafer[strip] == 0
    if (data.m_StripHitsOnWafer[strip] != 0) { // real hits already checked
      continue;
    }
    if (data.m_Analogue[1][strip] > 0) { // Better way of doing this?!
                                    // set data.m_StripHitsOnWafer to x in prepareGainAndOffset
      if (m_data_compression_mode == 1 and m_data_readout_mode == 0) { // level mode x1x
        if (data.m_Analogue[1][strip] < m_Threshold) {
          data.m_StripHitsOnWafer[strip] = -2; // Below threshold
        } else {
          data.m_StripHitsOnWafer[strip] = 2; // Crosstalk+Noise hit
          if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, 2)) {
            ATH_MSG_ERROR("Can't add noise hit diode to collection");
          }
        }
      } else if (m_data_compression_mode == 2 and m_data_readout_mode == 0) { // edge mode 01x
        if ((data.m_Analogue[0][strip] >= m_Threshold or data.m_Analogue[1][strip] < m_Threshold)) {
          data.m_StripHitsOnWafer[strip] = -2; // Below threshold
        } else {
          data.m_StripHitsOnWafer[strip] = 2; // Crosstalk+Noise hit
          if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, 2)) {
            ATH_MSG_ERROR("Can't add noise hit diode to collection");
          }
        }
      } else if (m_data_compression_mode == 3 or m_data_readout_mode == 1) {
        int have_hit_bin = 0;
        if (data.m_Analogue[0][strip] >= m_Threshold) {
          have_hit_bin = 4;
        }
        if (data.m_Analogue[1][strip] >= m_Threshold) {
          have_hit_bin += 2;
        }
        if (data.m_Analogue[2][strip] >= m_Threshold) {
          have_hit_bin += 1;
        }
        if (m_data_compression_mode == 1) { // !< level and expanded mode
          if (have_hit_bin == 2 or have_hit_bin == 3 or have_hit_bin == 6 or have_hit_bin == 7) {
            data.m_StripHitsOnWafer[strip] = 2; // Crosstalk+Noise hit
            if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, have_hit_bin)) {
              ATH_MSG_ERROR("Can't add noise hit diode to collection");
            }
          } else {
            data.m_StripHitsOnWafer[strip] = -2; // Below threshold
          }
        } else if (m_data_compression_mode == 2) { // !< edge and expanded mode
          if (have_hit_bin == 2 or have_hit_bin == 3) {
            data.m_StripHitsOnWafer[strip] = 2; // Noise hit
            if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, have_hit_bin)) {
              ATH_MSG_ERROR("Can't add noise hit diode to collection");
            }
          } else {
            data.m_StripHitsOnWafer[strip] = -2; // Below threshold
          }
        } else if (m_data_compression_mode == 3) { // !< any hit mode
          if (have_hit_bin == 0) {
            data.m_StripHitsOnWafer[strip] = -2; // Below threshold
          } else {
            data.m_StripHitsOnWafer[strip] = 2; // !< Crosstalk+Noise hit
            if (m_data_readout_mode == 1) { // !< check for exp mode or not
              if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, have_hit_bin)) {
                ATH_MSG_ERROR("Can't add noise hit diode to collection");
              }
            } else {
              if (StatusCode::SUCCESS != addNoiseDiode(collection, strip, 2)) {
                ATH_MSG_ERROR("Can't add noise hit diode to collection");
              }
            }
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_FrontEnd::doClustering(SiChargedDiodeCollection& collection, SCT_FrontEndData& data) const {
  // ********************************
  // now do clustering
  // ********************************
  int strip = 0;
  int clusterSize = 0;

  const SCT_ModuleSideDesign& sctDesign = dynamic_cast<const SCT_ModuleSideDesign&>(collection.design());

  Identifier hitStrip;

  if (m_data_readout_mode == 0) {
    do {
      if (data.m_StripHitsOnWafer[strip] > 0) {
        // ====== First step: Get the cluster size
        // ===================================================
        int clusterFirstStrip = strip;

        // Find end of cluster. In multi-row sensors, cluster cannot span rows.
        int row = sctDesign.row(strip);
        if (row < 0) {
          row = 0;
        }

        int lastStrip1DInRow = 0;
        for (int i = 0; i < row + 1; ++i) {
          lastStrip1DInRow += sctDesign.diodesInRow(i);
        }

        while (strip < lastStrip1DInRow-1 and data.m_StripHitsOnWafer[strip +1] > 0) {
          ++strip; // !< Find first strip hit and add up the following strips
        }
        int clusterLastStrip = strip;

        clusterSize = (clusterLastStrip - clusterFirstStrip) + 1;
        hitStrip = m_sct_id->strip_id(collection.identify(), clusterFirstStrip);
        SiChargedDiode& HitDiode = *(collection.find(hitStrip));
        SiHelper::SetStripNum(HitDiode, clusterSize, &msg());
                                                                                      
        SiChargedDiode *PreviousHitDiode = &HitDiode;
        for (int i = clusterFirstStrip+1; i <= clusterLastStrip; ++i) {
          hitStrip = m_sct_id->strip_id(collection.identify(), i);
          SiChargedDiode& HitDiode2 = *(collection.find(hitStrip));
          SiHelper::ClusterUsed(HitDiode2, true);
          PreviousHitDiode->setNextInCluster(&HitDiode2);
          PreviousHitDiode = &HitDiode2;
        }
      }
      ++strip; // !< This is the starting point of the next cluster within this collection
    } while (strip < m_strip_max);
  } else {
    // Expanded read out mode, basically one RDO/strip per cluster
    // But if consecutively fired strips have the same time bin, those are converted into one cluster.
    do {
      clusterSize = 1;
      if (data.m_StripHitsOnWafer[strip] > 0) {
        hitStrip = m_sct_id->strip_id(collection.identify(), strip);
        SiChargedDiode& hitDiode = *(collection.find(hitStrip));
        int timeBin = SiHelper::GetTimeBin(hitDiode);
        SiChargedDiode* previousHitDiode = &hitDiode;
        // Check if consecutively fired strips have the same time bin
        for (int newStrip=strip+1; newStrip<m_strip_max; newStrip++) {
          if (not (data.m_StripHitsOnWafer[newStrip]>0)) break;
          Identifier newHitStrip = m_sct_id->strip_id(collection.identify(), newStrip);
          SiChargedDiode& newHitDiode = *(collection.find(newHitStrip));
          if (timeBin!=SiHelper::GetTimeBin(newHitDiode)) break;
          SiHelper::ClusterUsed(newHitDiode, true);
          previousHitDiode->setNextInCluster(&newHitDiode);
          previousHitDiode = &newHitDiode;
          clusterSize++;
        }
        SiHelper::SetStripNum(hitDiode, clusterSize, &msg());

#ifdef SCT_DIG_DEBUG
        ATH_MSG_DEBUG("RDO Strip = " << strip << ", tbin = " <<
                      SiHelper::GetTimeBin(hitDiode) <<
                      ", HitInfo(1=real, 2=crosstalk, 3=noise): " <<
                      data.m_StripHitsOnWafer[strip]);
#endif
      }
      strip += clusterSize; // If more than one strip fires, those following strips are skipped.
    } while (strip < m_strip_max);
  }

  // clusters below threshold, only from pre-digits that existed before no
  // pure noise clusters below threshold will be made
  // D. Costanzo. I don't see why we should cluster the strips below
  // threshold. I'll pass on the info of strips below threshold
  // to the SDOs. I'll leave the SCT experts the joy to change this if they
  // don't like what I did or if this requires too much memory/disk

  return StatusCode::SUCCESS;
}

StatusCode FaserSCT_FrontEnd::addNoiseDiode(SiChargedDiodeCollection& collection, int strip, int tbin) const {
  const SiCellId ndiode(strip); // !< create a new diode
  const SiCharge noiseCharge(2 * m_Threshold, 0, SiCharge::noise); // !< add some noise to it
  collection.add(ndiode, noiseCharge); // !< add it to the collection

  // Get the strip back to check
  const Identifier idstrip = m_sct_id->strip_id(collection.identify(), strip);
  SiChargedDiode *NoiseDiode = (collection.find(idstrip));
  if (NoiseDiode == nullptr) {
    return StatusCode::FAILURE;
  }
  SiHelper::SetTimeBin(*NoiseDiode, tbin, &msg()); // set timebin info

  return StatusCode::SUCCESS;
}

float FaserSCT_FrontEnd::meanValue(std::vector<float>& calibDataVect) const {
  float mean_value = 0.0;
  int nData = 0;
  const unsigned int vec_size = calibDataVect.size();

  for (unsigned int i = 0; i < vec_size; ++i) {
    if (calibDataVect[i] > 0.1) {
      mean_value += calibDataVect[i];
      ++nData;
    }
  }

  if (nData == 0) {
    return -1;
  } else {
    return mean_value / nData;
  }
}
