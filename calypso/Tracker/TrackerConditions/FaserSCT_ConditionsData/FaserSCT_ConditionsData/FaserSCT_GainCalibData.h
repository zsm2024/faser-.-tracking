/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * FaserSCT_GainCalibData.h
 * @file header file for data object for gain parameters for all modules
 * @author Susumu Oda - 12/10/17
 **/

#ifndef FASERSCT_GAINCALIBDATA_H
#define FASERSCT_GAINCALIBDATA_H

#include "FaserSCT_ConditionsData/FaserSCT_ModuleGainCalibData.h"

// Definition of the number of elements
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"

typedef boost::array<FaserSCT_ModuleGainCalibData, FaserSCT_ConditionsData::NUMBER_OF_MODULES> FaserSCT_GainCalibData;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserSCT_GainCalibData , 6442222 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( FaserSCT_GainCalibData, 111415994 );

#endif // FASERSCT_GAINCALIBDATA_H
