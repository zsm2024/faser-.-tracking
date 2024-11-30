/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and FAsER collaborations
*/

/** @file ISCT_CableMappingTool.h Interface file for SCT_CableMappingTool.
 */

// Multiple inclusion protection
#ifndef ISCT_CABLE_MAPPING_TOOL
#define ISCT_CABLE_MAPPING_TOOL

//STL includes
#include <vector>

//Gaudi Includes
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventContext.h"

class ISCT_CableMappingTool: virtual public IAlgTool {

 public:
  
  //----------Public Member Functions----------//
  // Structors
  virtual ~ISCT_CableMappingTool() = default; //!< Destructor

  /// Creates the InterfaceID and interfaceID() method
  DeclareInterfaceID(ISCT_CableMappingTool, 1, 0);

  // Methods to return cable-mapping data 
  virtual std::map<int, std::pair<int,int> > getCableMapping(const EventContext& ctx) const = 0;
  virtual std::map<int, std::pair<int,int> > getCableMapping(void) const = 0;

};

//---------------------------------------------------------------------- 
#endif // ISCT_CABLE_MAPPING_TOOL
