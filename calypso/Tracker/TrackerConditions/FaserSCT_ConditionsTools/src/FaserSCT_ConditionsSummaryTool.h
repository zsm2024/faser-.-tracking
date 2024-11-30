#ifndef FASERSCT_CONDITIONSSUMMARYTOOL_H
#define FASERSCT_CONDITIONSSUMMARYTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "InDetConditionsSummaryService/InDetHierarchy.h"
#include "FaserSCT_ConditionsTools/ISCT_ConditionsSummaryTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/EventContext.h"

#include <string>
#include <vector>

class ISCT_ConditionsTool;

/**
 * @class FaserSCT_ConditionsSummaryTool
 * Interface class for tool providing summary of status of an SCT detector element
**/
class FaserSCT_ConditionsSummaryTool: public extends<AthAlgTool, ISCT_ConditionsSummaryTool> {
public:
  FaserSCT_ConditionsSummaryTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Tool constructor
  virtual ~FaserSCT_ConditionsSummaryTool() = default;
  virtual StatusCode initialize() override;

  virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h) const override;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, const InDetConditions::Hierarchy h) const override;
  virtual bool isGood(const IdentifierHash& elementHash) const override;
  virtual bool isGood(const IdentifierHash& elementHash, const EventContext& ctx) const override;

private:
  StringArrayProperty m_reportingTools; //!< list of tools to be used
  ToolHandleArray<ISCT_ConditionsTool> m_toolHandles;
  bool m_noReports{true};
};


#endif // FASERSCT_CONDITIONSSUMMARYTOOL_H
