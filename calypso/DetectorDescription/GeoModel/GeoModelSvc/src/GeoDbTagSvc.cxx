/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoDbTagSvc.h"
#include "RDBMaterialManager.h"
#include "GaudiKernel/ServiceHandle.h"

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "RDBAccessSvc/IRDBRecord.h"

GeoDbTagSvc::GeoDbTagSvc(const std::string& name,ISvcLocator* svc)
  : AthService(name,svc)
  , m_geoConfig(GeoModel::GEO_RUN3)
{
}

GeoDbTagSvc::~GeoDbTagSvc()
{
}

StatusCode GeoDbTagSvc::initialize()
{
  ATH_MSG_DEBUG("initialize()");
  return StatusCode::SUCCESS;
}

StatusCode GeoDbTagSvc::finalize()
{
  ATH_MSG_DEBUG("finalize()");
  return StatusCode::SUCCESS;
}

// Query the interfaces.
//   Input: riid, Requested interface ID
//          ppvInterface, Pointer to requested interface
//   Return: StatusCode indicating SUCCESS or FAILURE.
// N.B. Don't forget to release the interface after use!!!
StatusCode GeoDbTagSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if(IGeoDbTagSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface = (IGeoDbTagSvc*)this;
    addRef();
    return StatusCode::SUCCESS;
  } else  {
    // Interface is not directly available: try out a base class
    return AthService::queryInterface(riid, ppvInterface);
  }
}

StatusCode GeoDbTagSvc::setupTags()
{
  ATH_MSG_DEBUG("setupTags()");
  
  // Check if the Atlas version has already been set
  if(m_FaserVersion.empty()) {
    ATH_MSG_FATAL("FASER tag not set!");
    return StatusCode::FAILURE;
  }
  
  // Get RDBAccessSvc
  ServiceHandle<IRDBAccessSvc> rdbAccessSvc("RDBAccessSvc", name());
  if(rdbAccessSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve RDBAccessSvc");
    return StatusCode::FAILURE;
  }

  // Get subsystem tags
  m_NeutrinoVersion = (m_NeutrinoVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Neutrino", m_FaserVersion, "FASER", "FASERDD") 
		    : m_NeutrinoVersionOverride);

  m_EmulsionVersion = (m_EmulsionVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Emulsion", m_NeutrinoVersion, "Neutrino", "FASERDD") 
		    : m_EmulsionVersionOverride);


  m_ScintVersion = (m_ScintVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Scintillator", m_FaserVersion, "FASER", "FASERDD") 
		    : m_ScintVersionOverride);

  m_VetoVersion = (m_VetoVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Veto", m_ScintVersion, "Scintillator", "FASERDD") 
		    : m_VetoVersionOverride);

  m_VetoNuVersion = (m_VetoNuVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("VetoNu", m_ScintVersion, "Scintillator", "FASERDD") 
		    : m_VetoNuVersionOverride);

  m_TriggerVersion = (m_TriggerVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Trigger", m_ScintVersion, "Scintillator", "FASERDD") 
		    : m_TriggerVersionOverride);

  m_PreshowerVersion = (m_PreshowerVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Preshower", m_ScintVersion, "Scintillator", "FASERDD") 
		    : m_PreshowerVersionOverride);

  m_TrackerVersion = (m_TrackerVersionOverride.empty()
		   ? rdbAccessSvc->getChildTag("Tracker", m_FaserVersion, "FASER", "FASERDD")
		   : m_TrackerVersionOverride);

  m_SCTVersion = (m_SCTVersionOverride.empty()
      ? rdbAccessSvc->getChildTag("SCT", m_FaserVersion, "Tracker", "FASERDD")
      : m_SCTVersionOverride);

  m_DipoleVersion = (m_DipoleVersionOverride.empty()
      ? rdbAccessSvc->getChildTag("Dipole", m_FaserVersion, "Tracker", "FASERDD")
      : m_SCTVersionOverride);

  m_CaloVersion = (m_CaloVersionOverride.empty()
       ? rdbAccessSvc->getChildTag("Calorimeter", m_FaserVersion, "FASER", "FASERDD")
		   : m_CaloVersionOverride);

  m_EcalVersion = (m_EcalVersionOverride.empty() 
		    ? rdbAccessSvc->getChildTag("Ecal", m_CaloVersion, "Calorimeter", "FASERDD") 
		    : m_EcalVersionOverride);

  // m_MagFieldVersion = (m_MagFieldVersionOverride.empty()
	// 	       ? rdbAccessSvc->getChildTag("MagneticField",m_AtlasVersion,"ATLAS")
	// 	       : m_MagFieldVersionOverride);

  m_TrenchVersion = (m_TrenchVersionOverride.empty()
			  ? rdbAccessSvc->getChildTag("Trench", m_FaserVersion,"Trench", "FASERDD")
			  : m_TrenchVersionOverride);

  // Retrieve geometry config information (RUN1, RUN2, etc...)
  IRDBRecordset_ptr faserCommonRec = rdbAccessSvc->getRecordsetPtr("FaserCommon", m_FaserVersion, "FASER", "FASERDD");
  if(faserCommonRec->size()==0) {
    m_geoConfig = GeoModel::GEO_RUN3;
  }
  else {
    std::string configVal = (*faserCommonRec)[0]->getString("CONFIG");
    if(configVal=="RUN3")
      m_geoConfig = GeoModel::GEO_RUN3;
    else if(configVal=="RUN4")
      m_geoConfig = GeoModel::GEO_RUN4;
    else if(configVal=="TESTBEAM")
      m_geoConfig = GeoModel::GEO_TESTBEAM;
    else {
      ATH_MSG_FATAL("Unexpected value for geometry config read from the database: " << configVal);
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
