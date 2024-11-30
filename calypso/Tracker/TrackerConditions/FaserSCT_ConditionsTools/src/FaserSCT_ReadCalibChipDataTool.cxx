/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file FaserSCT_ReadCalibChipDataTool.cxx Implementation file for FaserSCT_ReadCalibChipDataTool.
    @author Per Johansson (23/03/09), Shaun Roe (17/2/2010)
*/

#include "FaserSCT_ReadCalibChipDataTool.h"

// Include Athena stuff
#include "Identifier/Identifier.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#include "FaserSCT_ConditionsData/FaserSCT_ModuleCalibParameter.h"
#include "FaserSCT_ConditionsData/FaserSCT_ModuleGainCalibData.h"
#include "FaserSCT_ConditionsData/FaserSCT_ModuleNoiseCalibData.h"
#include "FaserSCT_ConditionsTools/SCT_ReadCalibChipDefs.h"

using namespace FaserSCT_ConditionsData;
using namespace SCT_ReadCalibChipDefs;

//----------------------------------------------------------------------
FaserSCT_ReadCalibChipDataTool::FaserSCT_ReadCalibChipDataTool (const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

//----------------------------------------------------------------------
StatusCode 
FaserSCT_ReadCalibChipDataTool::initialize() {
  // Get SCT helper
  ATH_CHECK(detStore()->retrieve(m_id_sct, "FaserSCT_ID"));

  // Read Cond Handle Key
  ATH_CHECK(m_condKeyGain.initialize());
  ATH_CHECK(m_condKeyNoise.initialize());

  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTool::initialize()

//----------------------------------------------------------------------
StatusCode
FaserSCT_ReadCalibChipDataTool::finalize() {
  // Print where you are
  return StatusCode::SUCCESS;
} // SCT_ReadCalibChipDataTool::finalize()

//----------------------------------------------------------------------
//Can only report good/bad at side level
bool
FaserSCT_ReadCalibChipDataTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h==InDetConditions::SCT_SIDE);
}

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
FaserSCT_ReadCalibChipDataTool::isGood(const IdentifierHash& elementHashId, const EventContext& ctx) const {
  // Retrieve FaserSCT_NoiseCalibData pointer
  const FaserSCT_NoiseCalibData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In isGood, FaserSCT_NoiseCalibData cannot be retrieved");
    return true;
  }

  const unsigned int moduleIdx{elementHashId/SIDES_PER_MODULE};
  // Retrieve defect data from map
  const FaserSCT_ModuleNoiseCalibData& noiseOccData{(*condDataNoise)[moduleIdx]};

  // Retrieve the data
  const int i{noiseOccIndex("NoiseByChip")};
  if (i<0) {
    ATH_MSG_ERROR("This NoiseOccupancy noise data does not exist");
    return true;
  }
  const FaserSCT_ModuleCalibParameter& moduleNoiseData{noiseOccData[i]};

  // Calcuate module status
  // For now just simple check NO mean noise level
  // Chip could be 0 if bypassed, need to check
  const int side{static_cast<int>(elementHashId%2)};
  int chip{side*CHIPS_PER_SIDE};
  const int endChip{CHIPS_PER_SIDE+chip};
  int nChips{0};
  float sum{0.0};
  for (; chip!=endChip; ++chip) {
    float chipNoise{moduleNoiseData[chip]};
    if (chipNoise!=0.0) {
      sum+=chipNoise;
      ++nChips;
    }
  }
  const float meanNoiseValue{sum/nChips};
  ATH_MSG_DEBUG("Module mean noise: " << meanNoiseValue);
  return (meanNoiseValue < m_noiseLevel);
} //FaserSCT_ReadCalibChipDataTool::summary()

bool
FaserSCT_ReadCalibChipDataTool::isGood(const IdentifierHash& elementHashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementHashId, ctx);
}

//----------------------------------------------------------------------
// Returns a bool summary of the data
bool
FaserSCT_ReadCalibChipDataTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (h==InDetConditions::SCT_SIDE) { //Could do by chip too
    const IdentifierHash elementIdHash{m_id_sct->wafer_hash(elementId)};
    return isGood(elementIdHash, ctx);
  } else{
    // Not applicable for Calibration data
    ATH_MSG_WARNING("summary(): " << h << "good/bad is not applicable for Calibration data");
    return true;
  }
}

bool
FaserSCT_ReadCalibChipDataTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

//----------------------------------------------------------------------
std::vector<float> 
FaserSCT_ReadCalibChipDataTool::getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getNPtGainData()");
  std::vector<float> waferData;

  // Retrieve FaserSCT_GainCalibData pointer
  const FaserSCT_GainCalibData* condDataGain{getCondDataGain(ctx)};
  if (condDataGain==nullptr) {
    ATH_MSG_ERROR("In getNPtGainData, FaserSCT_GainCalibData cannot be retrieved");
    return waferData;
  }

  //find hash
  const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
  //make index
  const unsigned int idx{hashId/SIDES_PER_MODULE};
  //Retrieve defect data from map
  try {
    const FaserSCT_ModuleGainCalibData& wantedNPGData{condDataGain->at(idx)};
    //find the correct index for the required data
    const int dataIdx{nPtGainIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This N-point gain data: " << datatype << " does not exist");
      return waferData;
    }
    const FaserSCT_ModuleCalibParameter& moduleGains{wantedNPGData[dataIdx]};
    const int startOffset{side*CHIPS_PER_SIDE};
    const int endOffset{CHIPS_PER_SIDE+startOffset};
    FaserSCT_ModuleCalibParameter::const_iterator it{moduleGains.begin() + startOffset};
    FaserSCT_ModuleCalibParameter::const_iterator end{moduleGains.begin() + endOffset};
    // Returns the data for the wanted wafer
    if (*it != *it) return waferData;
    //could use signaling NaN here and catch the exception instead, would be quicker: NO! 
    //see: http://stackoverflow.com/questions/235386/using-nan-in-c
    waferData.assign(it, end);
    return waferData;
  } catch (const std::out_of_range& e) {
    return waferData;
  }
} //FaserSCT_ReadCalibChipDataTool::getNPtGainData()

std::vector<float> 
FaserSCT_ReadCalibChipDataTool::getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getNPtGainData(moduleId, side, datatype, ctx);
}

//----------------------------------------------------------------------
std::vector<float>
FaserSCT_ReadCalibChipDataTool::getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const {
  // Print where you are
  ATH_MSG_DEBUG("in getNoiseOccupancyData()");
  std::vector<float> waferData;

  // Retrieve FaserSCT_NoiseCalibData pointer
  const FaserSCT_NoiseCalibData* condDataNoise{getCondDataNoise(ctx)};
  if (condDataNoise==nullptr) {
    ATH_MSG_ERROR("In getNPtNoiseData, FaserSCT_NoiseCalibData cannot be retrieved");
    return waferData;
  }

  //find hash
  const IdentifierHash hashId{m_id_sct->wafer_hash(moduleId)};
  //make index
  const unsigned int idx{hashId/SIDES_PER_MODULE};
  try {
    //Retrieve defect data from array
    const FaserSCT_ModuleNoiseCalibData& wantedNoiseData{condDataNoise->at(idx)};

    //find the correct index for the required data
    const int dataIdx{noiseOccIndex(datatype)};
    if (dataIdx<0) {
      ATH_MSG_ERROR("This Noise Occupancy data: " << datatype << " does not exist");
      return waferData;
    }
    const FaserSCT_ModuleCalibParameter& moduleNoise{wantedNoiseData[dataIdx]};
    const int startOffset{side*CHIPS_PER_SIDE};
    const int endOffset{CHIPS_PER_SIDE+startOffset};
    FaserSCT_ModuleCalibParameter::const_iterator it{moduleNoise.begin() + startOffset};
    FaserSCT_ModuleCalibParameter::const_iterator end{moduleNoise.begin() + endOffset};
    // Returns the data for the wanted wafer
    if (*it != *it) return waferData;
    waferData.assign(it, end);
    return waferData;
  } catch (const std::out_of_range& e) {
    return waferData;
  }
} // FaserSCT_ReadCalibChipDataTool::getNoiseOccupancyData()

std::vector<float>
FaserSCT_ReadCalibChipDataTool::getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getNoiseOccupancyData(moduleId, side, datatype, ctx);
}

int
FaserSCT_ReadCalibChipDataTool::nPtGainIndex(const std::string& dataName) const {
  int i{N_NPTGAIN};
  while (i--) if (dataName==nPtGainParameterNames[i]) break;
  return i;
}

int
FaserSCT_ReadCalibChipDataTool::noiseOccIndex(const std::string& dataName) const {
  int i{N_NOISEOCC};
  while (i--) if (dataName==noiseOccParameterNames[i]) break;
  return i;
}

const FaserSCT_GainCalibData*
FaserSCT_ReadCalibChipDataTool::getCondDataGain(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_GainCalibData> condData{m_condKeyGain, ctx};
  return condData.retrieve();
}

const FaserSCT_NoiseCalibData*
FaserSCT_ReadCalibChipDataTool::getCondDataNoise(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_NoiseCalibData> condData{m_condKeyNoise, ctx};
  return condData.retrieve();
}
