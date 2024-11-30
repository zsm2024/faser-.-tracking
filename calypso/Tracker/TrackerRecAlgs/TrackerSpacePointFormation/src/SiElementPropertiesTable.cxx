/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 
 -------------------
 ATLAS Collaboration
 ***************************************************************************/

#include "SiElementPropertiesTable.h"

#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerReadoutGeometry/SiDetectorDesign.h"
#include "TrackerReadoutGeometry/SiLocalPosition.h" 

namespace TrackerDD{
SiElementPropertiesTable::SiElementPropertiesTable(const FaserSCT_ID&  idHelper,
    const TrackerDD::SiDetectorElementCollection& elements,
    float   epsilonWidth) 
    {
  size_t maxSCT = idHelper.wafer_hash_max();
  m_properties.assign(maxSCT, nullptr);
  for (size_t i = 0; i < maxSCT; ++i){
     IdentifierHash hash(i);
     const TrackerDD::SiDetectorElement* element = elements[hash]; 
     if (element != 0){ 
       SiElementProperties* props = new SiElementProperties(hash, idHelper,*element,epsilonWidth);
       m_properties[i] = props;
     }
  }
}
//--------------------------------------------------------------------------

SiElementPropertiesTable::~SiElementPropertiesTable(){
  size_t maxSCT = m_properties.size();
  for (size_t i=0; i < maxSCT; ++i){
    delete m_properties[i];m_properties[i] =0;
  }
}
}
//--------------------------------------------------------------------------

