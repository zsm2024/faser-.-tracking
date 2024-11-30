/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * FaserSCT_ModuleCalibParameter.h
 * @file header file for data object for a calibration parameter per chip of a module
 * @author Susumu Oda - 12/10/17
 **/

#ifndef FASERSCT_MODULECALIBPARAMETER_H
#define FASERSCT_MODULECALIBPARAMETER_H

// Include boost stuff
#include "boost/array.hpp"

// Definition of the number of elements
#include "FaserSCT_ConditionsData/FaserSCT_ConditionsParameters.h"

typedef boost::array<float, FaserSCT_ConditionsData::CHIPS_PER_MODULE> FaserSCT_ModuleCalibParameter;

// Class definition
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( FaserSCT_ModuleCalibParameter , 52360049 , 1 )

#endif // FASERSCT_MODULECALIBPARAMETER_H
