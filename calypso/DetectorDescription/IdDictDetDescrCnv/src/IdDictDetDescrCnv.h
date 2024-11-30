/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IDDICTDETDESCRCNV_IDDICTDETDESCRCNV_H
#define IDDICTDETDESCRCNV_IDDICTDETDESCRCNV_H
/**
 * @file IdDictDetDescrCnv.h
 *
 * @brief Converter for the DetDescrCnvSvc which parses the identifier
 * xml dictionaries and creates an IdDictManager in the
 * DetectorStore. This is used by the IdHelpers to initialize
 * themselves.
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "DetDescrCnvSvc/DetDescrConverter.h"

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdDictParser;
class IdDictManager;
//class IProperty;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class IdDictDetDescrCnv
 *
 * @brief Converter for the DetDescrCnvSvc which parses the identifier
 * xml dictionaries and creates an IdDictManager in the
 * DetectorStore. This is used by the IdHelpers to initialize
 * themselves.
 *
 */

class IdDictDetDescrCnv: public DetDescrConverter {

public:
    virtual long int   repSvcType() const;
    virtual StatusCode initialize();
    virtual StatusCode finalize();
    virtual StatusCode createObj(IOpaqueAddress* pAddr, DataObject*& pObj);

    /// Storage type and class ID (used by CnvFactory)
    static long int     storageType();
    static const CLID&  classID();

    IdDictDetDescrCnv(ISvcLocator* svcloc);

private:

    /// (re)Initialize the manager
    StatusCode initTheManager();


    /// Get file names from properties
    StatusCode getFileNamesFromProperties(IProperty* propertyServer);
    
    /// Get file names from properties
    StatusCode getFileNamesFromTags();

    /// Register the requested files with the xml parser
    StatusCode registerFilesWithParser();

    /// Register the requested files and tag with the created id dicts
    StatusCode registerInfoWithDicts();

    /// Get tag to use for dictionary
    StatusCode getTag(std::string& tag);

    /// Print out the contained dictionaries and version
    void printDicts(const IdDictManager* dictMgr);

    /// Create and (re)initialize the IdDictManager - only create the
    /// first time
    StatusCode parseXMLDescription();

    /// The xml parser for the dictionary descriptions
    IdDictParser*  m_parser;

    /// Flag to tell helpers to do Checks
    bool           m_doChecks;

    /// Flag to generate neighbor information - for calos
    bool           m_doNeighbours;

    /// Name of top-level xml dict file
    std::string    m_idDictName;

    /// Flag to get dict parameters from Relational DetDescr DB
    bool           m_idDictFromRDB;

    /// Flag to which determines whether the xml files are parsed or
    /// not 
    bool           m_doParsing;

    /// File to be read for top-level subsystem ids values
    std::string   m_faserIDFileName;

    /// File to be read for Neutrino ids
    std::string   m_neutrinoIDFileName;

    /// File to be read for Scint ids
    std::string   m_scintIDFileName;

    /// File to be read for Tracker ids
    std::string   m_trackerIDFileName;

    /// File to be read for Calo ids
    std::string   m_caloIDFileName;

    /// Files for Calo Neighbors
    // std::string   m_fullAtlasNeighborsName;
    // std::string   m_fcal2dNeighborsName;
    // std::string   m_fcal3dNeighborsNextName;
    // std::string   m_fcal3dNeighborsPrevName;
    // std::string   m_tileNeighborsName;

    /// File to be read for Muon ids
    // std::string   m_muonIDFileName;

    /// File to be read for Forward det ids
    // std::string   m_forwardIDFileName;

    /// Tag of RDB record for Atlas top-level ids
    std::string   m_faserIdDictTag;

    /// Tag of RDB record for Neutrino ids
    std::string   m_neutrinoIdDictTag;

    /// Tag of RDB record for Scint ids
    std::string   m_scintIdDictTag;

    /// Tag of RDB record for Tracker ids
    std::string   m_trackerIdDictTag;

    /// Tag of RDB record for Calo ids
    std::string   m_caloIdDictTag;

    /// Tag of RDB record for Muon ids
    // std::string   m_muonIdDictTag;

    /// Tag of RDB record for forwards det ids
    // std::string   m_forwardIdDictTag;

    /// Internal Scint id tag
    // std::string   m_scintIDTag;
    
};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // IDDICTDETDESCRCNV_IDDICTDETDESCRCNV_H
