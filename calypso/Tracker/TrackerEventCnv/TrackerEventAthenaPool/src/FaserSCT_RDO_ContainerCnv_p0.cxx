/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "FaserSCT_RDO_ContainerCnv_p0.h"



#include "TrackerIdentifier/FaserSCT_ID.h"
#include "TrackerRawData/FaserSCT_RDO_Container.h"
#include "TrackerRawData/FaserSCT_RDO_Collection.h"

//================================================================
FaserSCT_RDO_Container* FaserSCT_RDO_ContainerCnv_p0::createTransient(const FaserSCT_RDO_Container_p0* persObj, MsgStream& log) {
  std::unique_ptr<FaserSCT_RDO_Container> trans(std::make_unique<FaserSCT_RDO_Container>(m_sctId->wafer_hash_max()));

  FaserSCT_RDO_Container_p0::const_iterator it   = persObj->begin();
  FaserSCT_RDO_Container_p0::const_iterator last = persObj->end();
  for (; it != last; ++it) {
    
    // Old persistent format used collection templated on the specific raw data type
    const TrackerRawDataCollection<FaserSCT1_RawData>* rdoCollOld = *it;
    
    if (rdoCollOld==nullptr) {
      throw std::runtime_error("Got NULL collection reading old format FaserSCT_RDO_Container");
    }

    // Ugly cast...  The new format does not need it in its converters.
    const TrackerRawDataCollection<FaserSCT_RDORawData>* rdoColl = reinterpret_cast<const TrackerRawDataCollection<FaserSCT_RDORawData> *>(rdoCollOld);
    
    // Add to the container
    if (trans->addCollection( rdoColl, rdoColl->identifyHash() ).isFailure()) {
      log << MSG::FATAL << "[p0] SCT RDOs could not be added to the container!" << endmsg;
      throw std::runtime_error("FaserSCT_RDO_ContainerCnv_p0::createTransient(): SCT RDOs could not be added to the container!");
    }
  }

  return trans.release();
}

//================================================================
