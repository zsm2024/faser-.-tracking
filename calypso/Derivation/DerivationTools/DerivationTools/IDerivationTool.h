/*
  Copyright (C) 2021 CERN for the benefit of the FASER collaboration
*/

/**
 * @file IDerivationTool.h
 * Header file for the IDerivationTool class
 * @author Carl Gwilliam, 2021
 */


#ifndef DERIVATIONTOOLS_IDERIVATIONTOOL_H
#define DERIVATIONTOOLS_IDERIVATIONTOOL_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/ToolHandle.h"


///Interface for derivation tools
class IDerivationTool : virtual public IAlgTool 
{
public:

  // InterfaceID
  DeclareInterfaceID(IDerivationTool, 1, 0);

  virtual ~IDerivationTool() = default;

  // Apply skimming
  virtual bool passed() = 0;

  /// Apply thinning
  virtual StatusCode removeBranch() = 0;

  /// Apply augmentation
  virtual StatusCode addBranch() = 0;


private:
  // None

};




#endif //DERIVATIONTOOLS_IDERIVATIONTOOL_H
