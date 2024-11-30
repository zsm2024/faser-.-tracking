/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * FaserSCT_NoiseCalibData.h
 * @file header file for data object for noise parameters for all modules
 * @author Susumu Oda - 12/10/17
 **/

#ifndef FASERSCT_NOISECALIBDATA_H
#define FASERSCT_NOISECALIBDATA_H

#include "FaserSCT_ConditionsData/FaserSCT_ModuleNoiseCalibData.h"

// Definition of the number of elements
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"

typedef boost::array<FaserSCT_ModuleNoiseCalibData, FaserSCT_ConditionsData::NUMBER_OF_MODULES> FaserSCT_NoiseCalibData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserSCT_NoiseCalibData , 26585486 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( FaserSCT_NoiseCalibData , 44705704 );

#endif // FASERSCT_NOISECALIBDATA_H
