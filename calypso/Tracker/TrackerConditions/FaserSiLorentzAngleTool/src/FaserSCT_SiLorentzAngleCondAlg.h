/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FaserSCT_SiLorentzAngleCondAlg.h
 * @author Susumu.Oda@cern.ch
 **/
#ifndef FaserSCTSiLorentzAngleCondAlg_h
#define FaserSCTSiLorentzAngleCondAlg_h

// Athena includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"

// #include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "InDetConditionsSummaryService/ISiliconConditionsTool.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSFloatCondData.h"
#include "FaserSiLorentzAngleTool/FaserSiLorentzAngleCondData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ICondSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolHandle.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "MagFieldElements/FaserFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// forward declarations
namespace MagField {
  class IMagFieldSvc;
}
// #include "MagFieldInterfaces/IMagFieldSvc.h"

/**
 * @class FaserSCTSiLorentzAngleCondAlg
 * This class prepares SiLorentAngleCondData using FaserSCT_SiliconConditionsTool
 **/

class FaserSCT_SiLorentzAngleCondAlg: public AthReentrantAlgorithm
{
 public:
  FaserSCT_SiLorentzAngleCondAlg(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~FaserSCT_SiLorentzAngleCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;
  virtual StatusCode finalize() override;
  /** Make this algorithm clonable. */
  virtual bool isClonable() const override { return true; };

 private:
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_readKeyTemp{this, "ReadKeyTemp", "SCT_SiliconTempCondData", "Key of input SCT temperature"};
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_readKeyHV{this, "ReadKeyHV", "SCT_SiliconBiasVoltCondData", "Key of input SCT HV"};
  // SG::ReadCondHandleKey<CondAttrListCollection> m_readKeyBFieldSensor{this, "ReadKeyBFieldSensor", "/EXT/DCS/MAGNETS/SENSORDATA",  "Key of input B-field sensor"};
  // The /GLOBAL/BField/Maps folder is run-lumi folder and has just one IOV. The folder is not used for IOV determination.
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};
  SG::WriteCondHandleKey<FaserSiLorentzAngleCondData> m_writeKey{this, "WriteKey", "SCTSiLorentzAngleCondData", "Key of output SiLorentzAngleCondData"};

  // needed services
  ServiceHandle<ICondSvc> m_condSvc;
  ServiceHandle<MagField::IMagFieldSvc> m_magFieldSvc;

  ToolHandle<ISiliconConditionsTool> m_siConditionsTool{this, "SiConditionsTool", "FaserSCT_SiliconConditionsTool", "Tool to retrieve SCT silicon information"};

  // Properties
  // YOU NEED TO USE THE SAME PROPERTIES AS USED IN SCTLorentzAngleTool!!!
  DoubleProperty           m_temperature{this, "Temperature", -7., "Default temperature in Celcius."};
  DoubleProperty           m_temperatureMin{this, "TemperatureMin", -80., "Minimum temperature allowed in Celcius."};
  DoubleProperty           m_temperatureMax{this, "TemperatureMax", 100., "Maximum temperature allowed in Celcius."};
  DoubleProperty           m_deplVoltage{this, "DepletionVoltage", 70., "Default depletion voltage in Volt."};
  DoubleProperty           m_biasVoltage{this, "BiasVoltage", 150., "Default bias voltage in Volt."};
  DoubleProperty           m_nominalField{this, "NominalField", 2.0834*Gaudi::Units::tesla};
  BooleanProperty          m_useMagFieldSvc{this, "UseMagFieldSvc", true};
  // BooleanProperty          m_useMagFieldDcs{this, "UseMagFieldDcs", true};
  BooleanProperty          m_sctDefaults{this, "useSctDefaults", false};
  BooleanProperty          m_useGeoModel{this, "UseGeoModel", false};
  unsigned int             m_maxHash;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  Amg::Vector3D getMagneticField(MagField::FaserFieldCache& fieldCache, const TrackerDD::SiDetectorElement* element) const;

  // Read handle for conditions object to get the field cache
  SG::ReadCondHandleKey<FaserFieldCacheCondObj> m_fieldCondObjInputKey {this, "FaserFieldCacheCondObj", "fieldCondObj",
                                                                        "Name of the Magnetic Field conditions object key"};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif // FaserSCTSiLorentzAngleCondAlg_h
