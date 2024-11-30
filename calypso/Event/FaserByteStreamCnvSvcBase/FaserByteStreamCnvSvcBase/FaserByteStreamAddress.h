/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FASERBYTESTREAMCNVSVCBASE_FASERBYTESTREAMADDRESS_H
#define FASERBYTESTREAMCNVSVCBASE_FASERBYTESTREAMADDRESS_H

// Framework include files
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/EventContext.h"

#include <stdint.h>
#include <vector>

/**
 *  @class  ByteStreamAddress
 *  @brief  IOpaqueAddress for ByteStreamCnvSvc, with ROB ids
 *
 *  Package     : ByteStreamCnvSvcBase
 *
 *  description : Definition of RawEvent address class
 *                This class holds a vector of ROBIDs
 *  @author     : H. Ma
 *  Revision    : July 10, 2002, Use new eformat package
 *  Revision    : Sept 19, 2002, Store ROB IDs, to be used with ROBDataProviderSvc
 */

class FaserByteStreamAddress : public GenericAddress {
 public:
  // @brief Constructor
  FaserByteStreamAddress(const CLID& clid, const std::string& fname, const std::string& cname, int p1 = 0, int p2 = 0);
  // @brief Constructor
  FaserByteStreamAddress(const CLID& clid);

  // @brief Destructor
  virtual ~FaserByteStreamAddress() {}; //doesn't own event

  // @brief Add a rob id
  void add(uint32_t robid);

  // @brief Add event id
  void setEventContext(const EventContext& eid);

  // @brief access the ROB fragment IDs
  const std::vector<uint32_t>& getRobIDs() const;

  // @brief get event id
  const EventContext& getEventContext() const;

  // @brief storage type to be used by all bytestream converters
  static constexpr long storageType() { return 0x43; }

 private:
  // @brief it holds a vector of rob ids
  std::vector<uint32_t> m_robIDs;
  EventContext m_eid;
};

inline void FaserByteStreamAddress::setEventContext(const EventContext& eid) 
{ m_eid = eid; }

inline const EventContext& FaserByteStreamAddress::getEventContext() const
{ return m_eid; }

#endif
