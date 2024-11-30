/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MagFieldConditions/FaserFieldMapCondObj.h"

FaserFieldMapCondObj::FaserFieldMapCondObj() {}

FaserFieldMapCondObj::~FaserFieldMapCondObj() {}

// access to field map 
const MagField::FaserFieldMap*
FaserFieldMapCondObj::fieldMap () const
{
    return m_fieldMap.get();
}

// setter
void
FaserFieldMapCondObj::setFieldMap(std::unique_ptr<MagField::FaserFieldMap> fieldMap)
{
    m_fieldMap = std::move(fieldMap);
}

