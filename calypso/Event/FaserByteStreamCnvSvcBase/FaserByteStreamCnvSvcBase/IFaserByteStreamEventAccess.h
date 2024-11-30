/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERBYTESTREAMCNVSVCBASE_IBYTESTREAMEVENTACCESS_H
#define FASERBYTESTREAMCNVSVCBASE_IBYTESTREAMEVENTACCESS_H

//#include "ByteStreamData/RawEvent.h"
#include "GaudiKernel/IInterface.h"

/** @class IByteStreamEventAccess
 * @brief interface for accessing raw data .
 */
static const InterfaceID
IID_IFaserByteStreamEventAccess("IFaserByteStreamEventAccess", 2 , 0);

class IFaserByteStreamEventAccess: virtual public IInterface {
 public:
  /// Gaudi interface id
  static const InterfaceID& interfaceID() { return IID_IFaserByteStreamEventAccess; }

  /// pure virtual method for accessing RawEventWrite
  //virtual RawEventWrite* getRawEvent() = 0;

};

#endif
