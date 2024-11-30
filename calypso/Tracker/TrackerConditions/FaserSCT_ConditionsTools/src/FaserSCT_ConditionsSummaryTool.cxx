/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "FaserSCT_ConditionsSummaryTool.h"
#include "FaserSCT_ConditionsTools/ISCT_ConditionsTool.h"


FaserSCT_ConditionsSummaryTool::FaserSCT_ConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent) :
  base_class(type, name, parent), m_toolHandles{this} {
  declareProperty("ConditionsTools", m_toolHandles);
}

StatusCode FaserSCT_ConditionsSummaryTool::initialize() {
  ATH_CHECK(m_toolHandles.retrieve());
  m_noReports = m_toolHandles.empty();
  return StatusCode::SUCCESS;
}

bool FaserSCT_ConditionsSummaryTool::isGood(const Identifier& elementId, const EventContext& ctx, const InDetConditions::Hierarchy h) const {
  if (not m_noReports) {
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if (tool->canReportAbout(h) and (not tool->isGood(elementId, ctx, h))) return false;
    }
  }
  return true;
}

bool FaserSCT_ConditionsSummaryTool::isGood(const Identifier& elementId, const InDetConditions::Hierarchy h) const {
  return isGood(elementId, Gaudi::Hive::currentContext(), h);
}

bool FaserSCT_ConditionsSummaryTool::isGood(const IdentifierHash& elementHash, const EventContext& ctx) const {
  if (not m_noReports) {
    for (const ToolHandle<ISCT_ConditionsTool>& tool: m_toolHandles) {
      if ((tool->canReportAbout(InDetConditions::SCT_SIDE) or
           tool->canReportAbout(InDetConditions::SCT_MODULE)) and
          (not tool->isGood(elementHash, ctx))) {
        return false;
      }
    }
  }
  return true;
}

bool FaserSCT_ConditionsSummaryTool::isGood(const IdentifierHash& elementHash) const {
  return isGood(elementHash, Gaudi::Hive::currentContext());
}
