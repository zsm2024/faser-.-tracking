/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "MagFieldConditions/FaserFieldCacheCondObj.h"

FaserFieldCacheCondObj::FaserFieldCacheCondObj()
{}

FaserFieldCacheCondObj::~FaserFieldCacheCondObj() {}


bool
FaserFieldCacheCondObj::initialize(double dipFieldScale,
                                   const MagField::FaserFieldMap* fieldMap)
{
    m_dipoleFieldScale = dipFieldScale;
    m_fieldMap      = fieldMap;
    return (m_fieldMap) != nullptr; // return false if cast failed
}

