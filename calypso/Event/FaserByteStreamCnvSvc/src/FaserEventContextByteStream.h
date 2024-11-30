/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTCONTEXTBYTESTREAM_H
#define EVENTCONTEXTBYTESTREAM_H

/** @file EventContextByteStream.h
 *  @brief This file contains the class definition for the EventContextByteStream class.
 *  @author Hong Ma <hma@bnl.gov>
 **/

#include "GaudiKernel/IEvtSelector.h"

class FaserEventSelectorByteStream;
class IOpaqueAddress;

/** @class FaserEventContextByteStream
 *  @brief This class provides the Context for EventSelectorByteStream
 **/
class FaserEventContextByteStream : virtual public IEvtSelector::Context {

 public:
  /// Constructor
  FaserEventContextByteStream(const IEvtSelector* selector);
  /// Copy constructor
  FaserEventContextByteStream(const FaserEventContextByteStream& ctxt);
  /// Destructor
  virtual ~FaserEventContextByteStream();

  /// Inequality operator.
  virtual void* identifier() const;

 private:
  const IEvtSelector* m_evtSelector;
};

#endif
