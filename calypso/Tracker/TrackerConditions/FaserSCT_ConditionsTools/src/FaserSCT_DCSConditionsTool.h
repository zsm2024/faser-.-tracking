// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERSCT_DCSConditionsTool_h
#define FASERSCT_DCSConditionsTool_h
/**
 * @file FaserSCT_DCSConditionsTool.h
 *
 * @brief Header file for the FaserSCT_DCSConditionsTool class 
 *  in package FaserSCT_ConditionsTools
 *
 * @author A. R-Veronneau 26/02/07, Shaun Roe 4/4/2008
 **/

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSCT_ConditionsTools/ISCT_DCSConditionsTool.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "Identifier/Identifier.h"
#include "Identifier/IdentifierHash.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSFloatCondData.h"
#include "FaserSCT_ConditionsData/FaserSCT_DCSStatCondData.h"

// Gaudi
#include "Gaudi/Property.h"
#include "GaudiKernel/EventContext.h"

// STL
#include <string>

class FaserSCT_ID;

/**
 * Class to provide DCS information about modules from the COOL database
 **/
class FaserSCT_DCSConditionsTool: public extends<AthAlgTool, ISCT_DCSConditionsTool> {
  
public:
  FaserSCT_DCSConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~FaserSCT_DCSConditionsTool() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;
  
  /// @name Methods to be implemented from virtual baseclass methods, when introduced
  //@{
  ///Return whether this service can report on the hierarchy level (e.g. module, chip...)
  virtual bool canReportAbout(InDetConditions::Hierarchy h) const override;
  //returns the module ID (int), or returns 9999 (not a valid module number) if not able to report
  virtual Identifier getModuleID(const Identifier& elementId, InDetConditions::Hierarchy h) const;
  ///Summarise the result from the service as good/bad
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  ///is it good?, using wafer hash
  virtual bool isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual bool isGood(const IdentifierHash& hashId) const override;
  //Returns HV (0 if there is no information)
  virtual float modHV(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual float modHV(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  //Does the same for hashIds
  virtual float modHV(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual float modHV(const IdentifierHash& hashId) const override;
  //Returns temp0 (0 if there is no information)
  virtual float hybridTemperature(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual float hybridTemperature(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  //Does the same for hashIds
  virtual float hybridTemperature(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual float hybridTemperature(const IdentifierHash& hashId) const override;
  //Returns temp0 + correction for Lorentz angle calculation (0 if there is no information)
  virtual float sensorTemperature(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual float sensorTemperature(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  //Does the same for hashIds
  virtual float sensorTemperature(const IdentifierHash& hashId, const EventContext& ctx) const override;
  virtual float sensorTemperature(const IdentifierHash& hashId) const override;
  //@}
    
private:
  //Key for DataHandle
  BooleanProperty m_readAllDBFolders{this, "ReadAllDBFolders", true};
  BooleanProperty m_returnHVTemp{this, "ReturnHVTemp", true};
  FloatProperty m_barrel_correction{this, "TempBarrelCorrection", -3.7};
  FloatProperty m_ecInner_correction{this, "TempEcInnerCorrection", -13.1};
  FloatProperty m_ecOuter_correction{this, "TempEcOuterCorrection", -15.5};
  SG::ReadCondHandleKey<FaserSCT_DCSStatCondData> m_condKeyState{this, "CondKeyState", "SCT_DCSStatCondData", "SCT DCS state"};
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_condKeyHV{this, "CondKeyHV", "SCT_DCSHVCondData", "SCT DCS HV"};
  SG::ReadCondHandleKey<FaserSCT_DCSFloatCondData> m_condKeyTemp0{this, "CondKeyTemp0", "SCT_DCSTemp0CondData", "SCT DCS temperature on side 0"};
  const FaserSCT_ID* m_pHelper{nullptr};
  static const Identifier s_invalidId;
  static const float s_defaultHV;
  static const float s_defaultTemperature;
  const FaserSCT_DCSStatCondData* getCondDataState(const EventContext& ctx) const;
  const FaserSCT_DCSFloatCondData* getCondDataHV(const EventContext& ctx) const;
  const FaserSCT_DCSFloatCondData* getCondDataTemp0(const EventContext& ctx) const;
};

#endif // FaserSCT_DCSConditionsTool_h 
