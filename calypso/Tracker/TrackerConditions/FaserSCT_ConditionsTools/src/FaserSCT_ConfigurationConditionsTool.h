/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file FaserSCT_ConfigurationConditionsTool.h
 * header file for tool which reads SCT configuration from database
 * @author shaun.roe@cern.ch, gwilliam@mail.cern.ch
**/


#ifndef FASERSCT_CONFIGURATIONCONDITIONSTOOL_H
#define FASERSCT_CONFIGURATIONCONDITIONSTOOL_H

// Athena includes
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSCT_ConditionsTools/ISCT_ConfigurationConditionsTool.h"

#include "TrackerReadoutGeometry/SiDetectorElement.h"
#include "FaserSCT_ConditionsData/FaserSCT_ConfigurationCondData.h"
#include "StoreGate/ReadCondHandleKey.h"

// Gaudi includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ContextSpecificPtr.h"
#include "TrackerReadoutGeometry/SiDetectorElementCollection.h"

// STL includes
#include <string>

// Forward declarations
class FaserSCT_ID;

/**
 * @class FaserSCT_ConfigurationConditionsTool
 * Tool which reads FaserSCT_Configuration from the database
 * Deals with bad modules, bad links, strips out of the readout and bad strips
**/

class FaserSCT_ConfigurationConditionsTool: public extends<AthAlgTool, ISCT_ConfigurationConditionsTool> {
 public:

  //@name Tool methods
  //@{
  FaserSCT_ConfigurationConditionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~FaserSCT_ConfigurationConditionsTool() = default;
  virtual StatusCode initialize() override;
  //@}

  /**Can the tool report about the given component? (chip, module...)*/
  virtual bool                          canReportAbout(InDetConditions::Hierarchy h) const override;

  /**Is the detector element good?*/
  virtual bool                          isGood(const Identifier& elementId, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;
  virtual bool                          isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h=InDetConditions::DEFAULT) const override;

  /**Is it good?, using wafer hash*/
  virtual bool                          isGood(const IdentifierHash& hashId) const override;
  virtual bool                          isGood(const IdentifierHash& hashId, const EventContext& ctx) const override;

  /**List of bad modules*/
  virtual const std::set<Identifier>*   badModules(const EventContext& ctx) const override;
  virtual const std::set<Identifier>*   badModules() const override;
  /**List of bad strips*/
  virtual void                          badStrips(std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  virtual void                          badStrips(std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  /**List of bad strips for a given module*/
  virtual void                          badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  virtual void                          badStrips(const Identifier& moduleId, std::set<Identifier>& strips, bool ignoreBadModules=false, bool ignoreBadChips=false) const override;
  /**List of bad links*/
  virtual std::pair<bool, bool>         badLinks(const IdentifierHash& hash, const EventContext& ctx) const override;
  virtual std::pair<bool, bool>         badLinks(const IdentifierHash& hash) const override;
  /**Bad links for a given module*/
  virtual const std::map<IdentifierHash, std::pair<bool, bool>>* badLinks(const EventContext& ctx) const override;
  virtual const std::map<IdentifierHash, std::pair<bool, bool>>* badLinks() const override;
  /**List of bad chips*/
  virtual const std::map<Identifier, unsigned int>* badChips(const EventContext& ctx) const override;
  virtual const std::map<Identifier, unsigned int>* badChips() const override;
  /**Bad chips for a given module*/
  virtual unsigned int                  badChips(const Identifier& moduleId, const EventContext& ctx) const override;
  virtual unsigned int                  badChips(const Identifier& moduleId) const override;
  /** Get the chip number containing a particular strip*/
  virtual int                           getChip(const Identifier& stripId, const EventContext& ctx) const override;
  virtual int                           getChip(const Identifier& stripId) const override;

 private:
  SG::ReadCondHandleKey<FaserSCT_ConfigurationCondData> m_condKey{this, "CondKeyConfig", "FaserSCT_ConfigurationCondData", "SCT DAQ configuration"};
  SG::ReadCondHandleKey<TrackerDD::SiDetectorElementCollection> m_SCTDetEleCollKey{this, "SCTDetEleCollKey", "SCT_DetectorElementCollection", "Key of SiDetectorElementCollection for SCT"};

  const FaserSCT_ID* m_idHelper{nullptr};
  BooleanProperty m_checkStripsInsideModules{this, "checkStripsInsideModule", false, " Do we want to check if a strip is bad because it is inside a bad module"};

  /** Is a strip within a bad module*/
  bool                                  isStripInBadModule(const Identifier& stripId, const FaserSCT_ConfigurationCondData*) const;
  /** Is a wafer in a bad module*/
  bool                                  isWaferInBadModule(const Identifier& waferId, const EventContext& ctx) const;

  /**Is a chip with this Identifier good?*/
  bool isGoodChip(const Identifier& stripId, const EventContext& ctx) const;

  /** enum for constants*/
  enum {stripsPerChip=128, lastStrip=767, invalidChipNumber=-1};

  const FaserSCT_ConfigurationCondData* getCondData(const EventContext& ctx) const;
  const TrackerDD::SiDetectorElement* getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const;
};

#endif //FASERSCT_CONFIGURATIONCONDITIONSTOOL_H
