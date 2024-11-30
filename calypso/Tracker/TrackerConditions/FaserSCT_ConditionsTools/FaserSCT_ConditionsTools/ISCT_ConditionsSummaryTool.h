/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISCT_CONDITIONSSUMMARYTOOL_H
#define ISCT_CONDITIONSSUMMARYTOOL_H

#include "GaudiKernel/IInterface.h"
#include "Identifier/IdContext.h"

#include "InDetConditionsSummaryService/InDetHierarchy.h"

class Identifier;
class IdentifierHash;

/**
 * @class ISCT_ConditionsSummaryTool
 * Interface class for service providing summary of status of a detector element
**/

class ISCT_ConditionsSummaryTool: virtual public IInterface, virtual public IAlgTool {
public:
  virtual ~ISCT_ConditionsSummaryTool() = default;
  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_ConditionsSummaryTool, 1, 0);

  virtual bool isGood(const Identifier& elementId, const InDetConditions::Hierarchy h) const =0;
  virtual bool isGood(const Identifier& elementId, const EventContext& ctx, const InDetConditions::Hierarchy h) const =0;
  virtual bool isGood(const IdentifierHash& elementHash) const =0;
  virtual bool isGood(const IdentifierHash& elementHash, const EventContext& ctx) const =0;
};

#endif //ISCT_CONDITIONSSUMMARYTOOL_H
