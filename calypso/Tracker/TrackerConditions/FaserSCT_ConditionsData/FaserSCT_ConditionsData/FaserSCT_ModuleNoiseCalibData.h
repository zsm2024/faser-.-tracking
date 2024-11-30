/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * FaserSCT_ModuleNoiseCalibData.h
 * @file header file for data object for noise parameters per module
 * @author Susumu Oda - 12/10/17
 **/

#ifndef FASERSCT_MODULENOISECALIBDATA_H
#define FASERSCT_MODULENOISECALIBDATA_H

#include "FaserSCT_ConditionsData/FaserSCT_ModuleCalibParameter.h"

// Definition of the number of elements
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"

typedef boost::array<FaserSCT_ModuleCalibParameter, FaserSCT_ConditionsData::N_NOISEOCC> FaserSCT_ModuleNoiseCalibData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserSCT_ModuleNoiseCalibData , 48618992 , 1 )

#endif // FASERSCT_MODULENOISECALIBDATA_H
