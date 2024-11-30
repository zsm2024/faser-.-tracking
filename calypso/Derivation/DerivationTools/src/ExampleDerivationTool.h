/*
   Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/** @file TriggerStreamTool.h
 *  Header file for TriggerStreamTool.h
 *
 */
#ifndef DERIVATIONTOOLS_EXAMPLEDERIVATIONTOOL_H
#define DERIVATIONTOOLS_EXAMPLEDERIVATIONTOOL_H

//Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationTools/IDerivationTool.h"

//Gaudi
#include "GaudiKernel/ToolHandle.h"

//STL

class ExampleDerivationTool: public extends<AthAlgTool, IDerivationTool> {
 public:

  /// Normal constructor for an AlgTool; 'properties' are also declared here
 ExampleDerivationTool(const std::string& type, 
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
  
  /** Fraction of events to save */
  Gaudi::Property<float> m_fraction {this, "SaveFraction", 100, "Fraction of events to save"};

  /** Number of events processed */
  int m_events {0};

  /** Number of events selected */
  int m_passed {0};

};

#endif // WAVEDIGITOOLS_WAVEFORMDIGITISATIONTOOL_H
