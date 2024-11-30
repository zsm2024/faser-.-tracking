/*
  Copyright (C) 2002-2023 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserMCTruth/FaserTrackBarcodeInfo.h"

FaserTrackBarcodeInfo::FaserTrackBarcodeInfo(int uid, int bc, ISF::FaserISFParticle* baseIsp)
  : FaserVTrackInformation(BarcodeOnly)
  , m_theBaseISFParticle(baseIsp)
  , m_barcode(bc)
  , m_uniqueID(uid)
  , m_returnedToISF(false)
{
}

void FaserTrackBarcodeInfo::SetBaseISFParticle(ISF::FaserISFParticle* isp)
{
  m_theBaseISFParticle=isp;
}

void FaserTrackBarcodeInfo::SetReturnedToISF(bool returned)
{
  m_returnedToISF = returned;
}

