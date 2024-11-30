/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file FaserEventContextByteStream.cxx
 *  @brief This file contains the implementation for the FaserEventContextByteStream class.
 *  @author Peter van Gemmeren <gemmeren@bnl.gov>
 *  @author Hong Ma <hma@bnl.gov>
 **/

#include "FaserEventContextByteStream.h"

//________________________________________________________________________________
FaserEventContextByteStream::FaserEventContextByteStream(const IEvtSelector* selector) : m_evtSelector(selector) {
}
//________________________________________________________________________________
FaserEventContextByteStream::FaserEventContextByteStream(const FaserEventContextByteStream& ctxt) : IEvtSelector::Context(),
										     m_evtSelector(ctxt.m_evtSelector) {
}
//________________________________________________________________________________
FaserEventContextByteStream::~FaserEventContextByteStream() {
}
//________________________________________________________________________________
void* FaserEventContextByteStream::identifier() const {
  return((void*)(m_evtSelector));
}
