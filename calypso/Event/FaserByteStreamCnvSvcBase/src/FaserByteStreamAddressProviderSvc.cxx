/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddressProviderSvc.h"
#include "FaserByteStreamCnvSvcBase/FaserByteStreamAddress.h"
#include "FaserByteStreamCnvSvcBase/IFaserROBDataProviderSvc.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/TransientAddress.h"
#include "GaudiKernel/ListItem.h"

#include "GaudiKernel/IClassIDSvc.h"

//#include "eformat/SourceIdentifier.h"

/// Standard constructor
FaserByteStreamAddressProviderSvc::FaserByteStreamAddressProviderSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	 ::AthService(name, pSvcLocator),
	m_clidSvc("ClassIDSvc", name),
	m_storeID(StoreID::EVENT_STORE) {
  declareProperty("StoreID", m_storeID);
}
//________________________________________________________________________________
FaserByteStreamAddressProviderSvc::~FaserByteStreamAddressProviderSvc() {
}
//________________________________________________________________________________
StatusCode FaserByteStreamAddressProviderSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name());
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ClassIDSvc.");
      return(StatusCode::FAILURE);
   }
   if (m_storeID < 0 || m_storeID > StoreID::UNKNOWN) {
      ATH_MSG_FATAL("Invalid StoreID " << m_storeID);
      return(StatusCode::FAILURE);
   }
   ATH_MSG_INFO("initialized ");
   ATH_MSG_INFO("-- Will fill Store with id =  " << m_storeID);
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode FaserByteStreamAddressProviderSvc::finalize() {
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ClassIDSvc.");
   }
   return(::AthService::finalize());
}
//________________________________________________________________________________
StatusCode FaserByteStreamAddressProviderSvc::preLoadAddresses(StoreID::type id, tadList& tlist) {
   ATH_MSG_DEBUG("in preLoadAddress");
   if (id != m_storeID) {
      ATH_MSG_DEBUG("StoreID = " << id << " does not match required id (" << m_storeID << ") skip");
      return(StatusCode::SUCCESS);
   }

   for (std::vector<std::string>::const_iterator it = m_typeNames.begin(), it_e = m_typeNames.end();
	   it != it_e; it++) {
      ListItem item(*it);
      const std::string& t = item.type();
      const std::string& nm = item.name();
      CLID classid;
      if (!m_clidSvc->getIDOfTypeName(t, classid).isSuccess()) {
         ATH_MSG_WARNING("Cannot create TAD for (type, name)" << " no CLID for " << t << " " << nm);
      } else {
         SG::TransientAddress* tad = new SG::TransientAddress(classid, nm);
         tlist.push_back(tad);
         ATH_MSG_DEBUG("Created TAD for (type, clid, name)" << t << " " << classid << " " << nm);
         // save the clid and key.
         m_clidKey[classid].insert(nm);
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode FaserByteStreamAddressProviderSvc::updateAddress(StoreID::type id,
                                                       SG::TransientAddress* tad,
                                                       const EventContext& ctx) {
   if (id != m_storeID) {
      return(StatusCode::FAILURE);
   }
   CLID clid = tad->clID();
   std::string nm = tad->name();
   std::map<CLID, std::set<std::string> >::const_iterator it = m_clidKey.find(clid);
   if (it == m_clidKey.end() || (*it).second.count(nm) == 0) {
      return(StatusCode::FAILURE);
   }
   ATH_MSG_DEBUG("Creating address for " << clid << " " << nm);
   FaserByteStreamAddress* add = new FaserByteStreamAddress(clid, nm, "");
   add->setEventContext(ctx);
   tad->setAddress(add);
   return(StatusCode::SUCCESS);
}
