/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_ConfigurationConditionsTool.h"

// Athena includes
#include "TrackerIdentifier/FaserSCT_ID.h"
// #include "InDetReadoutGeometry/SiDetectorElement.h"
#include "StoreGate/ReadCondHandle.h"

// Constructor
FaserSCT_ConfigurationConditionsTool::FaserSCT_ConfigurationConditionsTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent)
{
}

// Initialize
StatusCode FaserSCT_ConfigurationConditionsTool::initialize() {
  ATH_MSG_DEBUG("Initializing configuration");

  ATH_CHECK(detStore()->retrieve(m_idHelper, "FaserSCT_ID"));

  // Read Cond Handle Key
  ATH_CHECK(m_condKey.initialize());
  ATH_CHECK(m_SCTDetEleCollKey.initialize());

  return StatusCode::SUCCESS;
}

// What level of element can this service report about
bool FaserSCT_ConfigurationConditionsTool::canReportAbout(InDetConditions::Hierarchy h) const {
  return (h == InDetConditions::SCT_STRIP or
          h == InDetConditions::SCT_CHIP or
          h == InDetConditions::SCT_SIDE or
          h == InDetConditions::SCT_MODULE or
          h == InDetConditions::DEFAULT);
}

// Is an element with this Identifier and hierarchy good?
bool FaserSCT_ConfigurationConditionsTool::isGood(const Identifier& elementId, const EventContext& ctx, InDetConditions::Hierarchy h) const {
  if (not canReportAbout(h)) return true;

  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In isGood, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return false;
  }

  bool result{true};
  if (h == InDetConditions::SCT_STRIP) {
    result = (not condData->isBadStrip(m_idHelper->wafer_hash(m_idHelper->wafer_id(elementId)),
                                       m_idHelper->strip(elementId)));

    ATH_MSG_VERBOSE("isGood: Strip " << m_idHelper->strip(elementId) << "/" << m_idHelper->wafer_hash(m_idHelper->wafer_id(elementId)) << " is " << result);

    // If strip itself is not bad, check if it's in a bad module
    if (result and m_checkStripsInsideModules) {
      result = (not isStripInBadModule(elementId, condData));
      ATH_MSG_VERBOSE("Good strip set bad by module check");
    }
  } else if (h == InDetConditions::SCT_MODULE) {
    result = (not condData->isBadModuleId(elementId));
  } else if (h == InDetConditions::SCT_SIDE or h == InDetConditions::DEFAULT) {
    result = (not condData->isBadWaferId(elementId));
  } else if (h == InDetConditions::SCT_CHIP) {
    result = isGoodChip(elementId, ctx);
  }
  return result;
}

bool FaserSCT_ConfigurationConditionsTool::isGood(const Identifier& elementId, InDetConditions::Hierarchy h) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};

  return isGood(elementId, ctx, h);
}

// Is a wafer with this IdentifierHash good?
bool FaserSCT_ConfigurationConditionsTool::isGood(const IdentifierHash& hashId, const EventContext& ctx) const {
  const Identifier elementId{m_idHelper->wafer_id(hashId)};
  return isGood(elementId, ctx, InDetConditions::SCT_SIDE);
}

bool FaserSCT_ConfigurationConditionsTool::isGood(const IdentifierHash& hashId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return isGood(hashId, ctx);
}

// Is a chip with this Identifier good?
bool FaserSCT_ConfigurationConditionsTool::isGoodChip(const Identifier& stripId, const EventContext& ctx) const {
  // This check assumes present SCT.
  // Get module number
  const Identifier moduleId{m_idHelper->module_id(stripId)};
  if (not moduleId.is_valid()) {
    ATH_MSG_WARNING("moduleId obtained from stripId " << stripId << " is invalid.");
    return false;
  }

  // badChips word for the module
  const unsigned int v_badChips{badChips(moduleId, ctx)};
  // badChips holds 12 bits.
  // bit 0 (LSB) is chip 0 for side 0.
  // bit 5 is chip 5 for side 0.
  // bit 6 is chip 6 for side 1.
  // bit 11 is chip 11 for side 1.

  // If there is no bad chip, this check is done.
  if (v_badChips==0) return true;

  const int side{m_idHelper->side(stripId)};
  // Check the six chips on the side
  // 0x3F  = 0000 0011 1111
  // 0xFC0 = 1111 1100 0000
  // If there is no bad chip on the side, this check is done.
  if ((side==0 and (v_badChips & 0x3F)==0) or (side==1 and (v_badChips & 0xFC0)==0)) return true;

  int chip{getChip(stripId, ctx)};
  if (chip<0 or chip>=12) {
    ATH_MSG_WARNING("chip number is invalid: " << chip);
    return false;
  }

  // Check if the chip is bad
  const bool badChip{static_cast<bool>(v_badChips & (1<<chip))};

  return (not badChip);
}

// Check if a strip is within a bad module
bool FaserSCT_ConfigurationConditionsTool::isStripInBadModule(const Identifier& stripId, const FaserSCT_ConfigurationCondData* condData) const {
  if (condData==nullptr) {
    ATH_MSG_ERROR("In isStripInBadModule, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return true;
  }

  const Identifier moduleId(m_idHelper->module_id(m_idHelper->wafer_id(stripId)));
  return condData->isBadModuleId(moduleId);
}

// Check if a wafer is within a bad module
bool FaserSCT_ConfigurationConditionsTool::isWaferInBadModule(const Identifier& waferId, const EventContext& ctx) const {
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In isWaferInBadModule, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return true;
  }

  const Identifier moduleId{m_idHelper->module_id(waferId)};
  return condData->isBadModuleId(moduleId);
}

// Find the chip number containing a particular strip Identifier
int FaserSCT_ConfigurationConditionsTool::getChip(const Identifier& stripId, const EventContext& ctx) const {
  // Find side and strip number
  const int side{m_idHelper->side(stripId)};
  int strip{m_idHelper->strip(stripId)};

  // Check for swapped readout direction
  const IdentifierHash waferHash{m_idHelper->wafer_hash(m_idHelper->wafer_id(stripId))};
  const TrackerDD::SiDetectorElement* pElement{getDetectorElement(waferHash, ctx)};
  if (pElement==nullptr) {
    ATH_MSG_FATAL("Element pointer is nullptr in 'badStrips' method");
    return invalidChipNumber;
  }
  strip = (pElement->swapPhiReadoutDirection()) ? lastStrip - strip: strip;

  // Find chip number
  return (side==0 ? strip/stripsPerChip : strip/stripsPerChip + 6);
}

int FaserSCT_ConfigurationConditionsTool::getChip(const Identifier& stripId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return getChip(stripId, ctx);
}

const std::set<Identifier>* FaserSCT_ConfigurationConditionsTool::badModules(const EventContext& ctx) const {
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badModules, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return nullptr;
  }

  return condData->getBadModuleIds();
}

const std::set<Identifier>* FaserSCT_ConfigurationConditionsTool::badModules() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badModules(ctx);
}

void FaserSCT_ConfigurationConditionsTool::badStrips(const Identifier& moduleId, std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules, bool ignoreBadChips) const {
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badStrips, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return;
  }

  // Bad strips for a given module
  if (ignoreBadModules) {
    // Ignore strips in bad modules
    if (condData->isBadModuleId(moduleId)) return;
  }

  for (const Identifier& badStripId: *(condData->getBadStripIds())) {
    if (ignoreBadChips) {
      // Ignore strips in bad chips
      const int chip{getChip(badStripId)};
      if (chip!=invalidChipNumber) {
        unsigned int chipStatusWord{condData->getBadChips(moduleId)};
        if ((chipStatusWord & (1 << chip)) != 0) continue;
      }
    }
    if (m_idHelper->module_id(m_idHelper->wafer_id(badStripId)) == moduleId) strips.insert(badStripId);
  }
}

void FaserSCT_ConfigurationConditionsTool::badStrips(const Identifier& moduleId, std::set<Identifier>& strips, bool ignoreBadModules, bool ignoreBadChips) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badStrips(moduleId, strips, ctx, ignoreBadModules, ignoreBadChips);
}

std::pair<bool, bool> FaserSCT_ConfigurationConditionsTool::badLinks(const IdentifierHash& hash, const EventContext& ctx) const {
  // Bad links for a given module
  // Bad convetion is used. true is for good link and false is for bad link...
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badLinks, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return std::pair<bool, bool>{false, false};
  }

  return condData->areBadLinks(hash);
}

std::pair<bool, bool> FaserSCT_ConfigurationConditionsTool::badLinks(const IdentifierHash& hash) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badLinks(hash, ctx);
}

const std::map<IdentifierHash, std::pair<bool, bool>>* FaserSCT_ConfigurationConditionsTool::badLinks(const EventContext& ctx) const {
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badLinks, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return nullptr;
  }

  return condData->getBadLinks();
}

const std::map<IdentifierHash, std::pair<bool, bool>>* FaserSCT_ConfigurationConditionsTool::badLinks() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badLinks(ctx);
}

const std::map<Identifier, unsigned int>* FaserSCT_ConfigurationConditionsTool::badChips(const EventContext& ctx) const {
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badChips, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return nullptr;
  }

  return condData->getBadChips();
}

const std::map<Identifier, unsigned int>* FaserSCT_ConfigurationConditionsTool::badChips() const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badChips(ctx);
}

unsigned int FaserSCT_ConfigurationConditionsTool::badChips(const Identifier& moduleId, const EventContext& ctx) const {
  // Bad chips for a given module
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badChips, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return 0xFFF; // 12 bad chips
  }

  return condData->getBadChips(moduleId);
}

unsigned int FaserSCT_ConfigurationConditionsTool::badChips(const Identifier& moduleId) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return badChips(moduleId, ctx);
}
void
FaserSCT_ConfigurationConditionsTool::badStrips(std::set<Identifier>& strips, const EventContext& ctx, bool ignoreBadModules, bool ignoreBadChips) const {
  const FaserSCT_ConfigurationCondData* condData{getCondData(ctx)};
  if (condData==nullptr) {
    ATH_MSG_ERROR("In badStrips, FaserSCT_ConfigurationCondData pointer cannot be retrieved");
    return;
  }

  if (!ignoreBadModules and !ignoreBadChips) {
    std::copy(condData->getBadStripIds()->begin(), condData->getBadStripIds()->end(), std::inserter(strips,strips.begin()));
    return;
  }
  for (const Identifier& badStripId: *(condData->getBadStripIds())) {
    const Identifier moduleId{m_idHelper->module_id(m_idHelper->wafer_id(badStripId))};
    // Ignore strips in bad modules
    if (ignoreBadModules) {
      if (condData->isBadModuleId(moduleId)) continue;
    }
    // Ignore strips in bad chips
    if (ignoreBadChips) {
      const int chip{getChip(badStripId)};
      if (chip!=invalidChipNumber) {
        unsigned int chipStatusWord{condData->getBadChips(moduleId)};
        if ((chipStatusWord & (1 << chip)) != 0) continue;
      }
    }
    strips.insert(badStripId);
  }
}

void
FaserSCT_ConfigurationConditionsTool::badStrips(std::set<Identifier>& strips, bool ignoreBadModules, bool ignoreBadChips) const {
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  badStrips(strips, ctx, ignoreBadModules, ignoreBadChips);
}

const FaserSCT_ConfigurationCondData*
FaserSCT_ConfigurationConditionsTool::getCondData(const EventContext& ctx) const {
  SG::ReadCondHandle<FaserSCT_ConfigurationCondData> condData{m_condKey, ctx};
  if (!condData.isValid()) {
    ATH_MSG_ERROR("can't retrieve condData");
  }
  return condData.retrieve();
}

const TrackerDD::SiDetectorElement* FaserSCT_ConfigurationConditionsTool::getDetectorElement(const IdentifierHash& waferHash, const EventContext& ctx) const {
  SG::ReadCondHandle<TrackerDD::SiDetectorElementCollection> condData{m_SCTDetEleCollKey, ctx};
  if (not condData.isValid()) return nullptr;
  return condData->getDetectorElement(waferHash);
}
