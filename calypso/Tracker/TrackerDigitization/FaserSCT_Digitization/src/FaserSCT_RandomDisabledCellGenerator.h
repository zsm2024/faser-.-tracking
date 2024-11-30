// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserSCT_RandomDisabledCellGenerator.h
//   Header file for class FaserSCT_RandomDisabledCellGenerator 
///////////////////////////////////////////////////////////////////
// Class to randomly disable cells for SCT
///////////////////////////////////////////////////////////////////
// Version 1.0 25/07/2007 Kondo Gnanvo
//   - Randomly select a cell and modify its flag by using the pointer to 
//     a SiHelper function.
//   - Inherits from ISiChargedDiodeProcessor, as SiPreDiodeProcessor
//     has been discontinued
// This can be used for disabling, disconnecting, and flagging bad_tot
///////////////////////////////////////////////////////////////////

#ifndef FASERSCT_DIGITIZATION_FASERSCT_RANDOMDISABLEDCELLGENERATOR_H
#define FASERSCT_DIGITIZATION_FASERSCT_RANDOMDISABLEDCELLGENERATOR_H

//Inheritance
#include "AthenaBaseComps/AthAlgTool.h"
#include "FaserSCT_Digitization/ISCT_RandomDisabledCellGenerator.h"

class SiChargedDiode;
class SiChargedDiodeCollection;

namespace CLHEP {
  class HepRandomEngine;
}

class FaserSCT_RandomDisabledCellGenerator : public extends<AthAlgTool, ISCT_RandomDisabledCellGenerator> {

  ///////////////////////////////////////////////////////////////////
  // Public methods:
  ///////////////////////////////////////////////////////////////////

 public:

  /**  constructor */
  FaserSCT_RandomDisabledCellGenerator(const std::string& type, const std::string& name, const IInterface* parent) ;

  /** Destructor */
  virtual ~FaserSCT_RandomDisabledCellGenerator() = default;

  /** AlgTool initialize */
  virtual StatusCode initialize() override;
  /** AlgTool finalize */
  virtual StatusCode finalize() override;

  virtual void process(SiChargedDiodeCollection& collection, CLHEP::HepRandomEngine * rndmEngine) const override;

 private:

  ///////////////////////////////////////////////////////////////////
  // Private data:
  ///////////////////////////////////////////////////////////////////
 private:

  FloatProperty m_disableProbability{this, "TotalBadChannels", 0.01, "probability that a cell is disabled"};

};

#endif //FASERSCT_DIGITIZATION_FASERSCT_RANDOMDISABLEDCELLGENERATOR_H
