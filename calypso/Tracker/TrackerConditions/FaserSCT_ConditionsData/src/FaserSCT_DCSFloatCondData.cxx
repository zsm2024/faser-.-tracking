/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Implementation file for the SCT DCS float data object class for HV and temperature
// The object is a map of channel number <-> float

#include "FaserSCT_ConditionsData/FaserSCT_DCSFloatCondData.h"

//////////////////////////////////
// constructor
FaserSCT_DCSFloatCondData::FaserSCT_DCSFloatCondData():
  m_channelValues{}
{
}

//////////////////////////////////
// set a float value for a channel
void FaserSCT_DCSFloatCondData::setValue(const CondAttrListCollection::ChanNum& chanNum, const float value) {
  m_channelValues[chanNum] = value;
}

//////////////////////////////////
// get the float value for a channel
bool FaserSCT_DCSFloatCondData::getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const {
  auto itr{m_channelValues.find(chanNum)};
  if(itr!=m_channelValues.end()) {
    value = itr->second;
    return true;
  }
  // the channel is not found.
  return false;
}

//////////////////////////////////
// clear
void FaserSCT_DCSFloatCondData::clear() {
  m_channelValues.clear();
}
