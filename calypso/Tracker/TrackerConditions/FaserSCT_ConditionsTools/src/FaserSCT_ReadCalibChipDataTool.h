// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file FaserSCT_ReadCalibChipDataTool.h Header file for FaserSCT_ReadCalibChipDataTool.
    @author Per Johansson, 23/03/09
*/

// Multiple inclusion protection
#ifndef FASERSCT_READ_CALIB_CHIP_DATA_TOOL
#define FASERSCT_READ_CALIB_CHIP_DATA_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSCT_ConditionsTools/ISCT_ReadCalibChipDataTool.h"

// Include Athena stuff
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"
#include "FaserSCT_ConditionsData/FaserSCT_GainCalibData.h"
#include "FaserSCT_ConditionsData/FaserSCT_NoiseCalibData.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

// Forward declarations
class FaserSCT_ID;

/** This class contains a Tool that reads SCT calibration data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class FaserSCT_ReadCalibChipDataTool: public extends<AthAlgTool, ISCT_ReadCalibChipDataTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  FaserSCT_ReadCalibChipDataTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~FaserSCT_ReadCalibChipDataTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  ///same thing with id hash, introduced by shaun with dummy method for now
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual bool isGood(const IdentifierHash& hashId) const override;
  //@}
  
  // Methods to return calibration data
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const override; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNPtGainData(const Identifier& moduleId, const int side, const std::string& datatype) const override; //!<Get NPtGain data per wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype, const EventContext& ctx) const override; //!<Get NoiseOccupancy data wafer
  virtual std::vector<float> getNoiseOccupancyData(const Identifier& moduleId, const int side, const std::string& datatype) const override; //!<Get NoiseOccupancy data wafer

 private:
  // Private enums
  enum FolderType {NPTGAIN, NOISEOCC, UNKNOWN_FOLDER, N_FOLDERTYPES};

  // Private methods
  int nPtGainIndex(const std::string& dataName) const;
  int noiseOccIndex(const std::string& dataName) const;

  const FaserSCT_GainCalibData* getCondDataGain(const EventContext& ctx) const;
  const FaserSCT_NoiseCalibData* getCondDataNoise(const EventContext& ctx) const;

  //----------Private Attributes----------//
  const FaserSCT_ID* m_id_sct{nullptr}; //!< Handle to FaserSCT ID helper
  
  // Read Cond Handles
  SG::ReadCondHandleKey<FaserSCT_GainCalibData> m_condKeyGain{this, "CondKeyGain", "SCT_GainCalibData", "SCT calibration data of gains of chips"};
  SG::ReadCondHandleKey<FaserSCT_NoiseCalibData> m_condKeyNoise{this, "CondKeyNoise", "SCT_NoiseCalibData", "SCT calibration data of noises of chips"};

  // Noise level for isGood::Side
  FloatProperty m_noiseLevel{this, "NoiseLevel", 1800.0, "Noise Level for isGood if ever used"};
};

//---------------------------------------------------------------------- 
#endif // FASERSCT_READ_CALIB_CHIP_DATA_TOOL
