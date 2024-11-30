/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDictDetDescrCnv package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>   $Id: IdDictDetDescrCnv.cxx,v 1.21 2009-02-15 13:08:19 schaffer Exp $
//<version>     $Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDictDetDescrCnv.h"

#include "IdDictParser/IdDictParser.h"
#include "IdDictDetDescr/IdDictManager.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelFaserUtilities/DecodeFaserVersionKey.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
 
#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h" 
#include "AthenaKernel/StorableConversions.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>


//--------------------------------------------------------------------

long int   
IdDictDetDescrCnv::repSvcType() const
{
  return (storageType());
}

//--------------------------------------------------------------------

StatusCode 
IdDictDetDescrCnv::initialize()
{
    // First call parent init
    StatusCode sc = DetDescrConverter::initialize();
    MsgStream log(msgSvc(), "IdDictDetDescrCnv");
    log << MSG::INFO << "in initialize" << endmsg;

    if (sc.isFailure()) {
        log << MSG::ERROR << "DetDescrConverter::initialize failed" << endmsg;
        return sc;
    }
    
    // Must set indet tag to EMPTY
    // m_scintIDTag = "EMPTY";
    
    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode 
IdDictDetDescrCnv::finalize()
{
    MsgStream log(msgSvc(), "IdDictDetDescrCnv");
    log << MSG::INFO << "in finalize" << endmsg;

    // Remove parser
    if(m_parser) delete m_parser;
    m_parser = 0;

    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

StatusCode
IdDictDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
    //
    // Here we create an IdDictManager and provide it with an
    // IdDictMgr which has been filled by an IdDictParser. This mgr
    // is used by each IdHelper to initialize itself. 
    //
    //   Lifetime management:
    //
    //     IdDictDetDescrCnv holds onto ONE IdDictParser, which in
    //     turn holds the same IdDictMgr. 
    //
    //     Multiple initializations are possible. parseXMLDescription
    //     will look for a new set of xml files, clear any
    //     pre-existing IdDictMgr help by the parser and then parse
    //     the new xml files, filling the IdDictMgr.
    //
    //     Since the parser "refills" the same IdDictMgr, one has the
    //     option to delete and recreate the IdDictManager, or just
    //     keep the same one which will be refreshed with the new
    //     description. 
    //
    MsgStream log(msgSvc(), "IdDictDetDescrCnv");
    log << MSG::INFO << "in createObj: creating a IdDictManager object in the detector store" << endmsg;

    DetDescrAddress* ddAddr;
    ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
    if(!ddAddr) {
        log << MSG::ERROR << "Could not cast to DetDescrAddress." << endmsg;
        return StatusCode::FAILURE;
    }

    // Get the StoreGate key of this container.
    std::string mgrKey  = *( ddAddr->par() );
    if ("" == mgrKey) {
        log << MSG::DEBUG << "No Manager key " << endmsg;
    }
    else {
        log << MSG::DEBUG << "Manager key is " << mgrKey << endmsg;
    }
    
    StatusCode sc = parseXMLDescription();
    if (sc != StatusCode::SUCCESS ) {
        log << MSG::ERROR << " Cannot parse the XML description " << endmsg; 
        return sc ;
    }


    // Create the manager - only once
    IdDictManager* dictMgr = new IdDictManager(m_parser->m_idd); 

    log << MSG::DEBUG << "Created IdDictManager " 
        << endmsg;
    

    // Print out the dictionary names
    printDicts(dictMgr);

    // Pass a pointer to the container to the Persistency service
    // by reference.
    pObj = SG::asStorable(dictMgr);

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

StatusCode
IdDictDetDescrCnv::parseXMLDescription() 
{
    MsgStream log(msgSvc(), "IdDictDetDescrCnv");

    log << MSG::DEBUG << "in getManager" << endmsg;

    // Parse the xml files to obtain the iddict dictionaries
    //
    //   Parsing of the xml files may not be needed. So we check. The
    //   conditions to reparse are:
    //
    //     - first pass, i.e. creating a new IdDictParser
    //     - a change in an input xml file
    //     - a change in one of the "options" such as doIdChecks,
    //       doInitNeighbors, etc.
    //
    
    m_doParsing = false; // Preset to no parsing

    if (!m_parser) {

        // Create parser
        m_parser    = new IdDictParser;
        m_doParsing = true; 
        
    }
    
    // We access the properties on each pass

    // Get the access to DetDescrCnvSvc for access to properties
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("DetDescrCnvSvc", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        log << MSG::ERROR 
            << " Cannot get DetDescrCnvSvc " 
            << endmsg; 
        return sc ;
    }

    // Set flag for doing checks of ids
    BooleanProperty     boolProperty("DoIdChecks", false);
    sc = propertyServer->getProperty(&boolProperty);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get DoIdChecks flag: found " 
            << boolProperty.value()
            << endmsg;
        return sc;
    }
    else {
        bool doChecks = boolProperty.value();
        if (doChecks != m_doChecks)m_doParsing = true; // Changed flag
        m_doChecks = doChecks;
        log << MSG::DEBUG << "Flag DoIdChecks is:  " 
            << m_doChecks
            << endmsg;
    }

    // Set flag for initializing neighbours
    BooleanProperty     boolProperty1("DoInitNeighbours", true);
    sc = propertyServer->getProperty(&boolProperty1);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get DoInitNeighbours flag: found " 
            << boolProperty1.value()
            << endmsg;
        return sc;
    }
    else {
        bool doNeighbours = boolProperty1.value();
        if (doNeighbours != m_doNeighbours)m_doParsing = true; // Changed flag
        m_doNeighbours = doNeighbours;
        log << MSG::DEBUG << "Flag DoInitNeighbours is:  " 
            << m_doNeighbours
            << endmsg;
    }

    // Name of IdDict file
    StringProperty property("IdDictName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get IdDictName: found " 
            << property.value()
            << endmsg;
        return sc;
    }
    else {
        std::string idDictName = property.value();
        if (idDictName != m_idDictName)m_doParsing = true; // Changed flag
        m_idDictName = idDictName;
        log << MSG::INFO << "IdDictName:  " 
            << m_idDictName
            << endmsg;
    }


    // Get the file names: two options - either from jobOpt
    // properties of the DetDescrCnvSvc or from RDB tags
        
    // Determine if Dict filename comes from DD database or
    // if properties from JobOptions should be used.
    BooleanProperty  boolProperty2("IdDictFromRDB", false);
    sc = propertyServer->getProperty(&boolProperty2);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get IdDictFromRDB: found " 
            << boolProperty2.value()
            << endmsg;
        return sc;
    } else {
        m_idDictFromRDB =  boolProperty2.value();           
        log << MSG::DEBUG << "IdDictFromRDB:  " 
            << (boolProperty2.value() ? "True" : "False")
            << endmsg;
    }
        
    if (m_idDictFromRDB) {
        log << MSG::DEBUG << "Dictonary file name from DD database" 
            << endmsg;
    } else {
        log << MSG::DEBUG << "Dictonary file name from job options or using defaults." 
            << endmsg;
    }
        
    // Get the file name to parse:
    //
    //   1) From Relational DB
    //   2) Via jobOptions
    //   3) default name in the xml files
    //
    //  Currently the logic is 1) or 2) and 3) covers the case where
    //  no file name is found.
    //
    if (m_idDictFromRDB) {

        // Get file names from RDB
        StatusCode sc = getFileNamesFromTags();
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to get Id file names from RDB " 
                << endmsg;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Looked for ID file name from RDB " 
                << endmsg;
        }

    }
    else {
        // Get file names from properties
        sc = getFileNamesFromProperties(propertyServer);
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to get Id file names from properties " 
                << endmsg;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Looked for ID file name from properties " 
                << endmsg;
        }
    }
        
    // Only parse if necessary
    if (m_doParsing) {
        

        // Register the requested files with the xml parser
        sc = registerFilesWithParser();
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to register file names " 
                << endmsg;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Registered file names " 
                << endmsg;
        }
        
        // Check whether a tag is needed for dictionary initialization

        // NOTE: the internal tag for IdDict is global, but is only
        // used for InDet and thus is defined by InDet
        std::string tag;
        // if (m_scintIDTag == "EMPTY") {
        //     sc = getTag(tag);
        //     if (!sc.isSuccess()) {
        //         log << MSG::DEBUG << " no tag found " 
        //             << endmsg;
        //         tag = "";  // force empty tag
        //     } 
        // } else {
        //     tag = m_scintIDTag;
        // }

        log << MSG::DEBUG << "Attempting to parse dictionary " << m_idDictName <<
            " with tag: " << ( tag == "" ? "default" : tag) << endmsg;

        // Parse the dictionaries
        m_parser->parse(m_idDictName.c_str(), tag);
        
        if (tag == "") tag = "default";
        log << MSG::DEBUG << "Read dict:  " 
            << m_idDictName << " with tag " << tag
            << endmsg;

        // Set flag to check ids
        IdDictMgr& mgr = m_parser->m_idd;
        if(m_doChecks) {
            mgr.set_do_checks(true);
            log << MSG::DEBUG << "Set IdDictManager doChecks flag to true " 
                << endmsg;
        }
        else {
            mgr.set_do_checks(false);
            log << MSG::DEBUG << "Set IdDictManager doChecks flag to false  " 
                << endmsg;
        }

        // Set flag to initialize neighbours
        if(m_doNeighbours) {
            mgr.set_do_neighbours(true);
            log << MSG::DEBUG << "Set IdDictManager doNeighbours flag to true " 
                << endmsg;
        }
        else {
            mgr.set_do_neighbours(false);
            log << MSG::DEBUG << "Set IdDictManager doNeighbours flag to false  " 
                << endmsg;
        }

        // Do some checks
        const IdDictMgr::dictionary_map& dm = mgr.get_dictionary_map (); 
        if (dm.begin () == dm.end ()) {
            // No dicts found
            log << MSG::ERROR << "No dictionaries found!" << endmsg;
            return StatusCode::FAILURE;
        }
        else {
            log << MSG::DEBUG << "Found " << dm.size() << " dictionaries." << endmsg;
        }

        // Register the requested files and tags with the id dicts
        sc = registerInfoWithDicts();
        if (!sc.isSuccess()) {
            log << MSG::ERROR << "unable to register info with dicts " 
                << endmsg;
            return sc;
        }
        else {
            log << MSG::DEBUG << "Registered info with id dicts " 
                << endmsg;
        }
    }
    else {
        log << MSG::WARNING << "NOTE:  ** parseXMLDescription called, but parsing was deemed unnecessary ** " 
            << endmsg;
    }
    

    log << MSG::DEBUG << "parseXMLDescription: Finished parsing and setting options " 
        << endmsg;

    return StatusCode::SUCCESS; 

}

//--------------------------------------------------------------------

StatusCode 
IdDictDetDescrCnv::initTheManager()
{
    return StatusCode::SUCCESS; 
}

//--------------------------------------------------------------------

long int
IdDictDetDescrCnv::storageType()
{
    return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& 
IdDictDetDescrCnv::classID() { 
    return ClassID_traits<IdDictManager>::ID(); 
}

//--------------------------------------------------------------------
IdDictDetDescrCnv::IdDictDetDescrCnv(ISvcLocator* svcloc) 
    :
    DetDescrConverter(ClassID_traits<IdDictManager>::ID(), svcloc),
    m_parser(0),
    m_doChecks(false),
    m_doNeighbours(true),
    m_idDictFromRDB(false),
    m_doParsing(false)
{}

//--------------------------------------------------------------------
StatusCode
IdDictDetDescrCnv::getTag(std::string& tag)
{

    MsgStream log(msgSvc(), "IdDictDetDescrCnv");


    // Allow the internal tag for dictionaries to be set by
    // jobOptions. Note that this is not used as far as I know. RDS
    // 10/2007. 
    log << MSG::DEBUG << "in getTag: chenk if tag is set in jobOpts" << endmsg;

    // Check whether we initialize from Zebra or ROOT
    IProperty* propertyServer(0); 
    StatusCode sc = serviceLocator()->service("DetDescrCnvSvc", propertyServer); 
    if (sc != StatusCode::SUCCESS ) {
        log << MSG::ERROR 
            << " Cannot get DetDescrCnvSvc " 
            << endmsg; 
        return sc ;
    }

    // Finally get tag from DetDescrCnvSvc property

    StringProperty tagProperty("IdDictGlobalTag", "");
    sc = propertyServer->getProperty(&tagProperty);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get IdDictGlobalTag: found " 
            << tagProperty.value()
            << endmsg;
    }
    else {
        tag = tagProperty.value();
        log << MSG::DEBUG << "Found IdDictGlobalTag:  " 
            << tag
            << endmsg;
    }

    return sc;
}
    
//--------------------------------------------------------------------
void    
IdDictDetDescrCnv::printDicts(const IdDictManager* dictMgr)
{
    MsgStream  log(msgSvc(),"IdDictDetDescrCnv");

    log << MSG::INFO 
        << "Found id dicts:" 
        << endmsg;

    if(!dictMgr) return;
    
    std::string tag = dictMgr->manager()->tag();
    if (!tag.size()) tag = "<no tag>";
    log << MSG::INFO << "Using dictionary tag: " << tag << endmsg;
    
    const IdDictMgr::dictionary_map& dm = dictMgr->manager()->get_dictionary_map (); 
    IdDictMgr::dictionary_map::const_iterator it;  
 
    int n = 0; 
 
    for (it = dm.begin (); it != dm.end (); ++it, ++n) { 
        const IdDictDictionary& dictionary = *((*it).second); 
        std::string version = ("" != dictionary.m_version) ? dictionary.m_version : "default";
        log << MSG::INFO 
            << "Dictionary " << dictionary.m_name;
        if (dictionary.m_name.size() < 20) {
            std::string space(20-dictionary.m_name.size(),' ');
            log << MSG::INFO << space;
        }
        log << MSG::INFO 
            << " version " << version;
        if (version.size() < 20) {
            std::string space(20-version.size(),' ');
            log << MSG::INFO << space;
        }
        if (dictionary.dict_tag().size()) {
            log << MSG::INFO 
                << " DetDescr tag " << dictionary.dict_tag();
            if (dictionary.dict_tag().size() < 20) {
                std::string space(25-dictionary.dict_tag().size(),' ');
                log << MSG::INFO << space;
            }
        }
        else {
            log << MSG::INFO 
                << " DetDescr tag (using default)";
        }
        log << MSG::INFO 
            << " file " << dictionary.file_name();
        log << MSG::INFO << endmsg;
    }
}

//--------------------------------------------------------------------
StatusCode 
IdDictDetDescrCnv::getFileNamesFromProperties(IProperty* propertyServer)
{

    // Check whether non-default names have been specified for the
    // IdDict files of the subsystems

    MsgStream log(msgSvc(), "IdDictDetDescrCnv");

    // Faser IDs
    StringProperty property = StringProperty("FaserIDFileName", "");
    StatusCode sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get FaserIDFileName: found " 
            << property.value()
            << endmsg;
        return sc;
    }
    else {
        m_faserIDFileName = property.value();
        std::string fileName = m_faserIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "FaserIDFileName:  " 
            << fileName
            << endmsg;
    }

    // Neutrino Ids
    property = StringProperty("NeutrinoIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get NeutrinoIDFileName: found " 
            << property.value()
            << endmsg;
        return sc;
    }
    else {
        m_neutrinoIDFileName = property.value();
        std::string fileName = m_neutrinoIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "NeutrinoIDFileName:  " 
            << fileName
            << endmsg;
    }

    // Scint Ids
    property = StringProperty("ScintIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get ScintIDFileName: found " 
            << property.value()
            << endmsg;
        return sc;
    }
    else {
        m_scintIDFileName = property.value();
        std::string fileName = m_scintIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "ScintIDFileName:  " 
            << fileName
            << endmsg;
    }

    // Tracker Ids
    property = StringProperty("TrackerIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get TrackerIDFileName: found " 
            << property.value()
            << endmsg;
        return sc;
    }
    else {
        m_trackerIDFileName = property.value();
        std::string fileName = m_trackerIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "TrackerIDFileName:  " 
            << fileName
            << endmsg;
    }

    // Calo ids 
    property = StringProperty("CaloIDFileName", "");
    sc = propertyServer->getProperty(&property);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "unable to get CaloIDFileName: found " 
            << property.value()
            << endmsg;
        return sc;
    }
    else {
        m_caloIDFileName = property.value();
        std::string fileName = m_caloIDFileName;
        if (fileName == "") fileName = "<not given>";
        log << MSG::DEBUG << "CaloIDFileName:  " 
            << fileName
            << endmsg;
    }

    // Calo neighbor files
    // property = StringProperty("FullAtlasNeighborsFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get FullAtlasNeighborsFileName: found " 
    //         << property.value() << endmsg;
    //     return sc;
    // }
    // else {
    //     m_fullAtlasNeighborsName = property.value();
    //     std::string fileName = m_fullAtlasNeighborsName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "FullAtlasNeighborsFileName:  " << fileName << endmsg;
    // }
    // property = StringProperty("FCAL2DNeighborsFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get FCAL2DNeighborsFileName: found " 
    //         << property.value() << endmsg;
    //     return sc;
    // }
    // else {
    //     m_fcal2dNeighborsName = property.value();
    //     std::string fileName = m_fcal2dNeighborsName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "FCAL2DNeighborsFileName:  " << fileName << endmsg;
    // }
    // property = StringProperty("FCAL3DNeighborsNextFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get FCAL3DNeighborsNextFileName: found " 
    //         << property.value() << endmsg;
    //     return sc;
    // }
    // else {
    //     m_fcal3dNeighborsNextName = property.value();
    //     std::string fileName = m_fcal3dNeighborsNextName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "FCAL3DNeighborsNextFileName:  " << fileName << endmsg;
    // }
    // property = StringProperty("FCAL3DNeighborsPrevFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get FCAL3DNeighborsPrevFileName: found " 
    //         << property.value() << endmsg;
    //     return sc;
    // }
    // else {
    //     m_fcal3dNeighborsPrevName = property.value();
    //     std::string fileName = m_fcal3dNeighborsPrevName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "FCAL3DNeighborsPrevFileName:  " << fileName << endmsg;
    // }
    // property = StringProperty("TileNeighborsFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get TileNeighborsFileName: found " 
    //         << property.value() << endmsg;
    //     return sc;
    // }
    // else {
    //     m_tileNeighborsName = property.value();
    //     std::string fileName = m_tileNeighborsName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "TileNeighborsFileName:  " << fileName << endmsg;
    // }

    // Muon ids
    // property = StringProperty("MuonIDFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get MuonIDFileName: found " 
    //         << property.value()
    //         << endmsg;
    //     return sc;
    // }
    // else {
    //     m_muonIDFileName = property.value();
    //     std::string fileName = m_muonIDFileName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "MuonIDFileName:  " 
    //         << fileName
    //         << endmsg;
    // }

    // ForwardDetectors ids
    // property = StringProperty("ForwardIDFileName", "");
    // sc = propertyServer->getProperty(&property);
    // if (!sc.isSuccess()) {
    //     log << MSG::ERROR << "unable to get ForwardIDFileName: found " 
    //         << property.value()
    //         << endmsg;
    //     return sc;
    // }
    // else {
    //     m_forwardIDFileName = property.value();
    //     std::string fileName = m_forwardIDFileName;
    //     if (fileName == "") fileName = "<not given>";
    //     log << MSG::DEBUG << "ForwardIDFileName:  " 
    //         << fileName
    //         << endmsg;
    // }

    return StatusCode::SUCCESS ;
}

    

//--------------------------------------------------------------------
StatusCode 
IdDictDetDescrCnv::getFileNamesFromTags()
{

    // Fetch file names and tags from the RDB 

    MsgStream log(msgSvc(), "IdDictDetDescrCnv");

    const IGeoModelSvc* geoModelSvc  = 0;
    IRDBAccessSvc*      rdbAccessSvc = 0;

    // Get GeoModelSvc and RDBAccessSvc in order to get the XML filenames and tags from the 
    // database
    StatusCode sc = service ("GeoModelSvc",geoModelSvc);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "Unable to get GeoModelSvc." << endmsg;
        return sc;
    }
    else log << MSG::DEBUG << "Accessed GeoModelSvc." << endmsg;
          
    sc = service("RDBAccessSvc",rdbAccessSvc);
    if (!sc.isSuccess()) {
        log << MSG::ERROR << "Unable to get RDBAccessSvc." << endmsg;
        return sc;
    }
    else log << MSG::DEBUG << "Accessed RDBAccessSvc." << endmsg;


    // RDS: not clear what a custom tag means.
    //     if (detectorKey.custom()) {
    //  // Get from properties - below. 
    //  inDetCustom = true;
    //} else {

    std::string        dictName;
    const IRDBRecord*  idDictTable = nullptr;

    // Get Neutrino
    DecodeFaserVersionKey detectorKey(geoModelSvc, "Neutrino");
    log << MSG::DEBUG << "From Version Tag: " 
	<< detectorKey.tag()  << " at Node: " << detectorKey.node() << endmsg;
    IRDBRecordset_ptr idDictSet   = rdbAccessSvc->getRecordsetPtr("NeutrinoIdentifier",
								  detectorKey.tag(), 
								  detectorKey.node(),
                                  "FASERDD");
    // Size == 0 if not found
    if (idDictSet->size()) {
      idDictTable       = (*idDictSet)[0];
      dictName          = idDictTable->getString("DICT_NAME");
      m_neutrinoIDFileName = idDictTable->getString("DICT_FILENAME");
      // NOTE: the internal tag for IdDict is global, but is
      // only used for InDet and thus is defined by InDet
    //   if(!idDictTable->isFieldNull("DICT_TAG")) {
    //     m_neutrinoIDTag    = idDictTable->getString("DICT_TAG");
    //   }
      m_neutrinoIdDictTag  = idDictSet->tagName();
      log << MSG::DEBUG << " using dictionary:  " << dictName 
	  << ", file: " <<  m_neutrinoIDFileName  
	//   << ", with internal tag: " << m_neutrinoIDTag
	  << ", dictionary tag: " << m_neutrinoIdDictTag
	  << endmsg;
    }
    else log << MSG::WARNING << " no record set found - using default dictionary " << endmsg;

    // Get Scint
    detectorKey = DecodeFaserVersionKey(geoModelSvc, "Scintillator");
    log << MSG::DEBUG << "From Version Tag: " 
	<< detectorKey.tag()  << " at Node: " << detectorKey.node() << endmsg;
    idDictSet   = rdbAccessSvc->getRecordsetPtr("ScintIdentifier",
								  detectorKey.tag(), 
								  detectorKey.node(),
                                  "FASERDD");
    // Size == 0 if not found
    if (idDictSet->size()) {
      idDictTable       = (*idDictSet)[0];
      dictName          = idDictTable->getString("DICT_NAME");
      m_scintIDFileName = idDictTable->getString("DICT_FILENAME");
      // NOTE: the internal tag for IdDict is global, but is
      // only used for InDet and thus is defined by InDet
    //   if(!idDictTable->isFieldNull("DICT_TAG")) {
    //     m_scintIDTag    = idDictTable->getString("DICT_TAG");
    //   }
      m_scintIdDictTag  = idDictSet->tagName();
      log << MSG::DEBUG << " using dictionary:  " << dictName 
	  << ", file: " <<  m_scintIDFileName  
	//   << ", with internal tag: " << m_scintIDTag
	  << ", dictionary tag: " << m_scintIdDictTag
	  << endmsg;
    }
    else log << MSG::WARNING << " no record set found - using default dictionary " << endmsg;
    
    // Get Tracker
    detectorKey = DecodeFaserVersionKey(geoModelSvc, "Tracker");
    log << MSG::DEBUG << "From Version Tag: " 
	<< detectorKey.tag()  << " at Node: " << detectorKey.node() << endmsg;
    idDictSet = rdbAccessSvc->getRecordsetPtr("TrackerIdentifier", 
					      detectorKey.tag(), 
					      detectorKey.node(),
                          "FASERDD");
    // Size == 0 if not found
    if (idDictSet->size()) {
      idDictTable      = (*idDictSet)[0];
      dictName         = idDictTable->getString("DICT_NAME");
      m_trackerIDFileName  = idDictTable->getString("DICT_FILENAME");
      m_trackerIdDictTag   = idDictSet->tagName();
      log << MSG::DEBUG << " using Dictionary:  " << dictName 
	  << ", file: " <<  m_trackerIDFileName
	  << ", dictionary tag: " << m_trackerIdDictTag
	  << endmsg;
    }
    else log << MSG::WARNING << " no record set found - using default dictionary " << endmsg;
    
    
    //Get Calo
    detectorKey = DecodeFaserVersionKey(geoModelSvc, "Calorimeter");
    log << MSG::DEBUG << "From Version Tag: " 
	<< detectorKey.tag()  << " at Node: " << detectorKey.node() << endmsg;
    idDictSet = rdbAccessSvc->getRecordsetPtr("CaloIdentifier",
					      detectorKey.tag(), 
					      detectorKey.node(),
                          "FASERDD");
    // Size == 0 if not found
    if (idDictSet->size()) {
      idDictTable       = (*idDictSet)[0];
      dictName          = idDictTable->getString("DICT_NAME");
      m_caloIDFileName  = idDictTable->getString("DICT_FILENAME");
      m_caloIdDictTag   = idDictSet->tagName();
      log << MSG::DEBUG << " using Dictionary:  " << dictName  
	  << ", file: " <<  m_caloIDFileName 
	  << ", dictionary tag: " << m_caloIdDictTag
	  << endmsg;
    }
    else log << MSG::WARNING << " no record set found - using default dictionary " << endmsg;                        

    log << MSG::DEBUG << "End access to RDB for id dictionary info " << endmsg;

    return StatusCode::SUCCESS ;
}

//--------------------------------------------------------------------
StatusCode 
IdDictDetDescrCnv::registerFilesWithParser()
{
    // If files names were found, register them with the parser to be read 

    MsgStream log(msgSvc(), "IdDictDetDescrCnv");

    if ("" != m_faserIDFileName) {
        m_parser->register_external_entity("FASER", m_faserIDFileName);
        log << MSG::INFO << "Reading FASER         IdDict file "
            << m_faserIDFileName
            << endmsg;
    }
    if ("" != m_neutrinoIDFileName) {
        m_parser->register_external_entity("Neutrino", m_neutrinoIDFileName);
        log << MSG::INFO << "Reading Neutrino      IdDict file "
            << m_neutrinoIDFileName
            << endmsg;
    }
    if ("" != m_scintIDFileName) {
        m_parser->register_external_entity("Scintillator", m_scintIDFileName);
        log << MSG::INFO << "Reading Scintillator  IdDict file "
            << m_scintIDFileName
            << endmsg;
    }
    if ("" != m_trackerIDFileName) {
        m_parser->register_external_entity("Tracker", m_trackerIDFileName);
        log << MSG::INFO << "Reading Tracker       IdDict file "
            << m_trackerIDFileName
            << endmsg;
    }
    if ("" != m_caloIDFileName) {
        m_parser->register_external_entity("Calorimeter", m_caloIDFileName);
        log << MSG::INFO << "Reading Calorimeter   IdDict file "
            << m_caloIDFileName
            << endmsg;
    }

    return StatusCode::SUCCESS ;
}

//--------------------------------------------------------------------
StatusCode
IdDictDetDescrCnv::registerInfoWithDicts()
{
    // Save the file name and tag in each of the dictionaries
    
    MsgStream log(msgSvc(), "IdDictDetDescrCnv");

    IdDictMgr& mgr = m_parser->m_idd;

    if ("" != m_faserIDFileName) {
        // Find Faser:
        IdDictDictionary* dict = mgr.find_dictionary("FASER");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for FASER" 
                << endmsg;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_faserIDFileName);
        dict->set_dict_tag (m_faserIdDictTag);
        log << MSG::DEBUG << "For FASER idDict, setting file/tag: "
            << m_faserIDFileName << " " << m_faserIdDictTag
            << endmsg;
    }
    if ("" != m_neutrinoIDFileName) {
        // Find Neutrino:
        IdDictDictionary* dict = mgr.find_dictionary("Neutrino");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for Neutrino" 
                << endmsg;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_neutrinoIDFileName);
        dict->set_dict_tag (m_neutrinoIdDictTag);
        log << MSG::DEBUG << "For Neutrino idDict, setting file/tag: "
            << m_neutrinoIDFileName << " " << m_neutrinoIdDictTag
            << endmsg;
    }
    if ("" != m_scintIDFileName) {
        // Find Scint:
        IdDictDictionary* dict = mgr.find_dictionary("Scintillator");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for Scintillator" 
                << endmsg;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_scintIDFileName);
        dict->set_dict_tag (m_scintIdDictTag);
        log << MSG::DEBUG << "For Scintillator idDict, setting file/tag: "
            << m_scintIDFileName << " " << m_scintIdDictTag
            << endmsg;
    }
    if ("" != m_trackerIDFileName) {
        // Find Tracker:
        IdDictDictionary* dict = mgr.find_dictionary("Tracker");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for Tracker" 
                << endmsg;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_trackerIDFileName);
        dict->set_dict_tag (m_trackerIdDictTag);
        log << MSG::DEBUG << "For Tracker idDict, setting file/tag: "
            << m_trackerIDFileName << " " << m_trackerIdDictTag
            << endmsg;
    }
    if ("" != m_caloIDFileName) {
        // Find Calo:
        IdDictDictionary* dict = mgr.find_dictionary("Calorimeter");  
        if (!dict) {
            log << MSG::ERROR 
                << "unable to find idDict for Calorimeter" 
                << endmsg;
            return StatusCode::FAILURE;
        }
        dict->set_file_name(m_caloIDFileName);
        dict->set_dict_tag (m_caloIdDictTag);
        log << MSG::DEBUG << "For Calorimeter idDict, setting file/tag: "
            << m_caloIDFileName << " " << m_caloIdDictTag
            << endmsg;
    }
    // if ("" != m_fullAtlasNeighborsName) {
    //     // Set neighbor file name:
    //     mgr.add_metadata("FULLATLASNEIGHBORS", m_fullAtlasNeighborsName);  
    //     log << MSG::DEBUG << "Added to dict mgr meta data: <FULLATLASNEIGHBORS, " 
    //         << m_fullAtlasNeighborsName << endmsg;
    // }
    // if ("" != m_fcal2dNeighborsName) {
    //     // Set neighbor file name:
    //     mgr.add_metadata("FCAL2DNEIGHBORS", m_fcal2dNeighborsName);  
    //     log << MSG::DEBUG << "Added to dict mgr meta data: <FCAL2DNEIGHBORS, " 
    //         << m_fcal2dNeighborsName << endmsg;
    // }
    // if ("" != m_fcal3dNeighborsNextName) {
    //     // Set neighbor file name:
    //     mgr.add_metadata("FCAL3DNEIGHBORSNEXT", m_fcal3dNeighborsNextName);  
    //     log << MSG::DEBUG << "Added to dict mgr meta data: <FCAL3DNEIGHBORSNEXT, " 
    //         << m_fcal3dNeighborsNextName << endmsg;
    // }
    // if ("" != m_fcal3dNeighborsPrevName) {
    //     // Set neighbor file name:
    //     mgr.add_metadata("FCAL3DNEIGHBORSPREV", m_fcal3dNeighborsPrevName);  
    //     log << MSG::DEBUG << "Added to dict mgr meta data: <FCAL3DNEIGHBORSPREV, " 
    //         << m_fcal3dNeighborsPrevName << endmsg;
    // }
    // if ("" != m_tileNeighborsName) {
    //     // Set neighbor file name:
    //     mgr.add_metadata("TILENEIGHBORS", m_tileNeighborsName);  
    //     log << MSG::DEBUG << "Added to dict mgr meta data: <TILENEIGHBORS, " 
    //         << m_tileNeighborsName << endmsg;
    // }
    // if ("" != m_muonIDFileName) {
    //     // Find Muon:
    //     IdDictDictionary* dict = mgr.find_dictionary("MuonSpectrometer");  
    //     if (!dict) {
    //         log << MSG::ERROR 
    //             << "unable to find idDict for MuonSpectrometer" 
    //             << endmsg;
    //         return StatusCode::FAILURE;
    //     }
    //     dict->set_file_name(m_muonIDFileName);
    //     dict->set_dict_tag (m_muonIdDictTag);
    //     log << MSG::DEBUG << "For MuonSpectrometer idDict, setting file/tag: "
    //         << m_muonIDFileName << " " << m_muonIdDictTag
    //         << endmsg;
    // }
    // if ("" != m_forwardIDFileName) {
    //     // Find Forward:
    //     IdDictDictionary* dict = mgr.find_dictionary("ForwardDetectors");  
    //     if (!dict) {
    //         log << MSG::ERROR 
    //             << "unable to find idDict for ForwardDetectors" 
    //             << endmsg;
    //         return StatusCode::FAILURE;
    //     }
    //     dict->set_file_name(m_forwardIDFileName);
    //     dict->set_dict_tag (m_forwardIdDictTag);
    //     log << MSG::DEBUG << "For ForwardDetectors idDict, setting file/tag: "
    //         << m_forwardIDFileName << " " << m_forwardIdDictTag
    //         << endmsg;
    // }

    return StatusCode::SUCCESS ;
}

