/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * FaserSCT_ModuleGainCalibData.h
 * @file header file for data object for gain parameters per module
 * @author Susumu Oda - 12/10/17
 **/

#ifndef FASERSCT_MODULEGAINCALIBDATA_H
#define FASERSCT_MODULEGAINCALIBDATA_H

#include "FaserSCT_ConditionsData/FaserSCT_ModuleCalibParameter.h"

// Definition of the number of elements
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"

typedef boost::array<FaserSCT_ModuleCalibParameter, FaserSCT_ConditionsData::N_NPTGAIN> FaserSCT_ModuleGainCalibData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserSCT_ModuleGainCalibData , 145481824 , 1 )

#endif // FASERSCT_MODULEGAINCALIBDATA_H
