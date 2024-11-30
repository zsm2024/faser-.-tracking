/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "DetDescrCnvSvc/DetDescrCnvSvc.h"
#include "DetDescrCnvSvc/IDetDescrCnvSvc.h"
//#include "AthenaServices/AthenaConversionSvc.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"


#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "StoreGate/StoreGateSvc.h"
//  #include "SGTools/TransientAddress.h"

//External definitions
long DetDescr_StorageType=0x44;

//-------------------------------------------------------------------------

/// Standard constructor
DetDescrCnvSvc::DetDescrCnvSvc(const std::string& name, ISvcLocator* svc)
        :
        ConversionSvc( name, svc, DetDescr_StorageType),
        m_detStore(0),
        m_decodeIdDict(true),
        m_idDictFromRDB(false),
        m_fromRoot(false),
        m_fromNova(false),
        m_detElemsfromDetNodes(false),
        m_compact_ids_only(false),
        m_do_checks(false),
        m_do_neighbours(true)
{
    declareProperty("DetectorManagers",            m_detMgrs);
    declareProperty("DetectorNodes",   	           m_detNodes );
    declareProperty("DecodeIdDict",	               m_decodeIdDict );
    declareProperty("IdDictName",   	           m_idDictName );
    declareProperty("IdDictFromRDB",   	           m_idDictFromRDB );
    declareProperty("IdDictGlobalTag",             m_idDictGlobalTag );
    declareProperty("ReadFromROOT",   	           m_fromRoot );
    declareProperty("ReadFromNova",   	           m_fromNova );
    declareProperty("InitDetElemsFromGeoModel",    m_detElemsfromDetNodes);
    declareProperty("CompactIDsOnly",              m_compact_ids_only);
    declareProperty("DoIdChecks",                  m_do_checks);
    declareProperty("DoInitNeighbours",            m_do_neighbours);

    declareProperty("FaserIDFileName",             m_idDictFASERName);
    declareProperty("NeutrinoIDFileName",          m_idDictNeutrinoName);
    declareProperty("ScintIDFileName",             m_idDictScintName);
    declareProperty("TrackerIDFileName",           m_idDictTrackerName);
    declareProperty("CaloIDFileName",              m_idDictCaloName);
}

//-------------------------------------------------------------------------

/// Standard Destructor
DetDescrCnvSvc::~DetDescrCnvSvc()   {
}

//-------------------------------------------------------------------------

/// Identify interfaces to which this service is responsive
StatusCode
DetDescrCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
    if ( riid == IConversionSvc::interfaceID() )  {
        *ppvInterface = (IConversionSvc*)this;
    }
    else if ( riid == IDetDescrCnvSvc::interfaceID() )  {
        *ppvInterface = (IDetDescrCnvSvc*)this;
    }
    else   {
        return ConversionSvc::queryInterface( riid, ppvInterface );
    }
    addRef();
    return StatusCode::SUCCESS;
}

//-------------------------------------------------------------------------

/// Initialize the service.
StatusCode
DetDescrCnvSvc::initialize()     {

    MsgStream log(msgSvc(),name());
    StatusCode status = ConversionSvc::initialize();
    if (status.isFailure()) {
	log << MSG::FATAL << "Unable to initialize ConversionSvc !" << endmsg;
	return StatusCode::FAILURE;
    }

    log << MSG::INFO << " initializing " <<endmsg ;

    // get DetectorStore service
    status = service("DetectorStore", m_detStore);
    if (status.isFailure()) {
	log << MSG::FATAL << "DetectorStore service not found !" << endmsg;
	return StatusCode::FAILURE;
    } else {
	log << MSG::INFO << "Found DetectorStore service" << endmsg;
    }

    // fill in the Addresses for Transient Detector Store objects

    log << MSG::INFO << " filling proxies for detector managers " <<endmsg ;

    // IdDict:
    status =  addToDetStore(2411, "IdDict");
    if (status != StatusCode::SUCCESS) return status;

    // IdHelpers
    status =  addToDetStore(125694213, "FaserID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(89852815,  "EmulsionID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(131395045, "VetoID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(58382802, "TriggerID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(55179317, "PreshowerID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(247779284, "VetoNuID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(205618430, "FaserSCT_ID");
    if (status != StatusCode::SUCCESS) return status;
    status =  addToDetStore(113753346, "EcalID");
    if (status != StatusCode::SUCCESS) return status;

    return status;
}

//-------------------------------------------------------------------------

StoreGateSvc *
DetDescrCnvSvc::detStore() const
{
    return (m_detStore);
}

//-------------------------------------------------------------------------

/// Create a Generic address using explicit arguments to identify a single object.
StatusCode DetDescrCnvSvc::createAddress(long            /* svc_type */,
					 const CLID&          /* clid     */,
					 const std::string*   /* par      */,
					 const unsigned long* /* ip       */,
					 IOpaqueAddress*& refpAddress)    {
    refpAddress = 0;
    return StatusCode::FAILURE;
}

//-------------------------------------------------------------------------

StatusCode DetDescrCnvSvc::createAddress( long /* svc_type */,
					  const CLID& clid,
					  const std::string& refAddress,
					  IOpaqueAddress*& refpAddress)
{
    try {
	refpAddress = new DetDescrAddress(clid);
	DetDescrAddress* ddAddr;
	ddAddr = dynamic_cast<DetDescrAddress*> (refpAddress);
	if(!ddAddr) {
	    MsgStream log(msgSvc(),name());
	    log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
	    return StatusCode::FAILURE;
	}
	StatusCode sc = ddAddr->fromString(refAddress);
    if (sc.isFailure())
    {
        MsgStream log(msgSvc(), name());
        log << MSG::FATAL << "Unable to create opaque address" << endmsg;
        return sc;
    }
    }
    catch(...) {
	refpAddress = 0;
    }
    return (refpAddress != 0) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

//-------------------------------------------------------------------------

StatusCode DetDescrCnvSvc::convertAddress( const IOpaqueAddress* pAddress,
                                           std::string& refAddress)
{
  const DetDescrAddress* addr = dynamic_cast<const DetDescrAddress*>(pAddress);
  if (!addr) return StatusCode::FAILURE;
  return addr->toString(refAddress);
}


//-------------------------------------------------------------------------

StatusCode
DetDescrCnvSvc::addToDetStore(const CLID& clid, const std::string& name) const
{

    MsgStream log(msgSvc(), "DetDescrCnvSvc");

    // Based on input parameters, create StoreGate proxies with
    // DetDescrAddresses in the detector store for the different
    // detectors.

    // fill in the Addresses for Transient Detector Store objects

    DetDescrAddress * addr;
    addr  = new DetDescrAddress(clid, name, name);
    StatusCode status = m_detStore->recordAddress(addr);
    if (status != StatusCode::SUCCESS) {
	log << MSG::INFO
	    << " unable to fill address for " << (*addr->par())
	    << " with CLID " << addr->clID()
	    << " and storage type " << addr->svcType()
	    << " to detector store " <<endmsg ;
	return StatusCode::FAILURE;
    }
    else {
	unsigned int st = addr->svcType();
	log << MSG::INFO
	    << " filling address for " << (*addr->par())
	    << " with CLID " << addr->clID()
	    << " and storage type " << st
	    << " to detector store " <<endmsg ;
    }
    return status;
}

