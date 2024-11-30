/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file TriggerStreamTool.h
 *  Header file for TriggerStreamTool.h
 *
 */
#ifndef DERIVATIONTOOLS_TRIGGERSTREAMTOOL_H
#define DERIVATIONTOOLS_TRIGGERSTREAMTOOL_H


// FASER
#include "StoreGate/ReadHandleKey.h"
#include "xAODFaserTrigger/FaserTriggerData.h"

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationTools/IDerivationTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL

class TriggerStreamTool: public extends<AthAlgTool, IDerivationTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
 TriggerStreamTool(const std::string& type, 
			  const std::string& name, const IInterface* parent);

  /// Retrieve the necessary services in initialize
  StatusCode initialize();

  // Apply skimming
  bool passed();

  /// Apply thinning
  StatusCode removeBranch() {return StatusCode::SUCCESS;}

  /// Apply augmentation
  StatusCode addBranch() {return StatusCode::SUCCESS;}

 private:

 /** Trigger condition to apply */
  Gaudi::Property<int> m_mask {this, "TriggerMask", 0x8, "Trigger mask to apply"};

  /// StoreGate key
 SG::ReadHandleKey<xAOD::FaserTriggerData> m_triggerDataKey
    { this, "FaserTriggerDataKey", "FaserTriggerData", "ReadHandleKey for xAOD::FaserTriggerData"};

};

#endif // WAVEDIGITOOLS_WAVEFORMDIGITISATIONTOOL_H
