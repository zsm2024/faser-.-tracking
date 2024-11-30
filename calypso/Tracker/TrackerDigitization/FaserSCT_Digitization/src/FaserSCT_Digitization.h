// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/** @file FaserSCT_Digitization.h  Header file for FaserSCT_Digitization class.
 *  @author Szymon.Gadomski@cern.ch, Awatif.Belymam@cern.ch, Davide.Costanzo@cern.ch,
 *          tgcornel@nikhef.nl, Grant.Gorfine@cern.ch, Paul.Bell@cern.ch,
 *          Jorgen.Dalmau@cern.ch, Kondo.Gnanvo@cern.ch, and others
 * Version 23/08/2007 Kondo.Gnanvo@cern.ch
 *          Conversion of the processors into AlgTool
 */

// Multiple inclusion protection
#ifndef FASERSCT_DIGITIZATION_FASERSCT_DIGITIZATION_H
#define FASERSCT_DIGITIZATION_FASERSCT_DIGITIZATION_H

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"
// Gaudi
#include "GaudiKernel/ToolHandle.h"

class IPileUpTool;

/** Top algorithm class for SCT digitization */
class FaserSCT_Digitization : public AthAlgorithm {

 public:

  /** Constructor with parameters */
  FaserSCT_Digitization(const std::string& name, ISvcLocator* pSvcLocator);

  /** Destructor */
  virtual ~FaserSCT_Digitization() = default;

  /** Basic algorithm methods */
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual bool isClonable() const override final { return true; }

 private:

  ToolHandle<IPileUpTool> m_sctDigitizationTool{this, "DigitizationTool", "FaserSCT_DigitizationTool", "SCT_DigitizationTool name"};

};

#endif // FASERSCT_DIGITIZATION_FASERSCT_DIGITIZATION_H
