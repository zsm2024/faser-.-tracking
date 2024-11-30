// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS and CERN collaborations
*/

/** @file FaserSCT_CableMappingTool.h Header file for FaserSCT_CableMappingTool.
    @author Dave Casper, 06/01/22
*/

// Multiple inclusion protection
#ifndef FASERSCT_CABLE_MAPPING_TOOL
#define FASERSCT_CABLE_MAPPING_TOOL

// Include interface class
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSCT_ConditionsTools/ISCT_CableMappingTool.h"

// Include Athena stuff
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "StoreGate/ReadCondHandleKey.h"

#include "GaudiKernel/ICondSvc.h"
#include "Gaudi/Property.h"

// #include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"
// #include "FaserSCT_ConditionsData/FaserSCT_GainCalibData.h"
// #include "FaserSCT_ConditionsData/FaserSCT_NoiseCalibData.h"

// Include Gaudi classes
#include "GaudiKernel/EventContext.h"

/** This class contains a Tool that reads SCT cable mapping data and makes it available to 
    other algorithms. The current implementation reads the data from a COOL database. 
*/

class FaserSCT_CableMappingTool: public extends<AthAlgTool, ISCT_CableMappingTool> {

 public:
  //----------Public Member Functions----------//
  // Structors
  FaserSCT_CableMappingTool(const std::string& type, const std::string& name, const IInterface* parent); //!< Constructor
  virtual ~FaserSCT_CableMappingTool() = default; //!< Destructor

  // Standard Gaudi functions
  virtual StatusCode initialize() override; //!< Gaudi initialiser
  virtual StatusCode finalize() override; //!< Gaudi finaliser

  // Methods to return calibration data
  virtual std::map<int, std::pair<int, int> > getCableMapping(const EventContext& ctx) const override;
  virtual std::map<int, std::pair<int, int> > getCableMapping(void) const override;

 private:
  // Read Cond Handle
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/SCT/DAQ/CableMapping", "Key of input cabling folder"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};

};

//---------------------------------------------------------------------- 
#endif // FASERSCT_CABLE_MAPPING_TOOL
