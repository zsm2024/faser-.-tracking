/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class FaserFieldMapCondAlg
   @authors RD Schaffer <r.d.schaffer -at- cern.ch>, C Anastopoulos
   @brief create a MagField object for tracking clients

   Purpose: create a MagField map object, filling from a file whose name is taken normally from
   COOL, and save the map in a conditions object for the FaserFieldCacheCondAlg

**/

// ISF_Services include
#include "FaserFieldMapCondAlg.h"

// Concurrency
#include "GaudiKernel/ConcurrencyFlags.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

MagField::FaserFieldMapCondAlg::FaserFieldMapCondAlg(const std::string& name, 
                                                   ISvcLocator* pSvcLocator)
    :AthReentrantAlgorithm(name, pSvcLocator){ 
}

MagField::FaserFieldMapCondAlg::~FaserFieldMapCondAlg(){ }

StatusCode
MagField::FaserFieldMapCondAlg::initialize() {

    ATH_MSG_INFO ("Initialize");
    // CondSvc
    ATH_CHECK( m_condSvc.retrieve() );

    // Read Handle for the map
    ATH_CHECK( m_mapsInputKey.initialize() );

    // Output handle for the field map
    ATH_CHECK( m_mapCondObjOutputKey.initialize() );

    // Register write handles for scale factors/cache and the field map
    if (m_condSvc->regHandle(this, m_mapCondObjOutputKey).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_mapCondObjOutputKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }
    
    ATH_MSG_INFO ( "Initialize: Key " << m_mapCondObjOutputKey.fullKey() << " has been succesfully registered " );
    if (m_useMapsFromCOOL) {
        ATH_MSG_INFO ( "Initialize: Will update the field map from conditions"); // 
    }
    else {
        ATH_MSG_INFO ( "Initialize: Will update the field map from jobOpt file name");
    }
    return StatusCode::SUCCESS;
}

StatusCode
MagField::FaserFieldMapCondAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG ( "execute: entering  ");

    // Check if output conditions object with field map object is still valid, if not replace it
    // with new map
    SG::WriteCondHandle<FaserFieldMapCondObj> mapWriteHandle{m_mapCondObjOutputKey, ctx};
    if (mapWriteHandle.isValid()) {
        ATH_MSG_DEBUG("execute: CondHandle " << mapWriteHandle.fullKey() 
                      << " is still valid. ");
        return StatusCode::SUCCESS;
    }  

    //This will need to be filled before we construct the condition object 
    Cache cache{};

    ATH_CHECK( updateFieldMap(ctx, cache) );


    ATH_MSG_INFO ( "execute: dipole zone id  " << cache.m_fieldMap->dipoleZoneId());
    
    
    // Save newly created map in conditions object, and record it in the conditions store, with its
    // own range
    auto fieldMapCondObj = std::make_unique<FaserFieldMapCondObj>();
    // move ownership of the field map to the fieldMapCondObj
    fieldMapCondObj->setFieldMap(std::move(cache.m_fieldMap));
    if(mapWriteHandle.record(cache.m_mapCondObjOutputRange, std::move(fieldMapCondObj)).isFailure()) {
        ATH_MSG_ERROR("execute: Could not record FaserFieldMapCondObj object with " 
                      << mapWriteHandle.key() 
                      << " with EventRange " << cache.m_mapCondObjOutputRange
                      << " into Conditions Store");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO ( "execute: recored FaserFieldMapCondObj with field map");

    return StatusCode::SUCCESS;
}

StatusCode
MagField::FaserFieldMapCondAlg::updateFieldMap(const EventContext& ctx, Cache& cache) const 
{
    // We get here only for the first creation of the field map, or if the FaserFieldMapCondObj
    // object, where the map is stored, is no longer valid, i.e. the IOV is out of range

    // Update the map either with the file names from cool or from alg properties, according to m_useMapsFromCOOL

    // Get file names from COOL, or use local ones:
    std::string fullMapFilename;

    if (m_useMapsFromCOOL) {

        ATH_MSG_INFO("updateFieldMap: Update map from conditions");
        
        // readin map file name from cool
        SG::ReadCondHandle<CondAttrListCollection> readHandle {m_mapsInputKey, ctx};
        const CondAttrListCollection* attrListColl{*readHandle};
        if (attrListColl == nullptr) {
            ATH_MSG_ERROR("updateFieldMap: Failed to retrieve CondAttributeListCollection with key " << readHandle.key());
            return StatusCode::FAILURE;
        }

        // // handle for COOL field map filenames
        // const DataHandle<CondAttrListCollection> mapHandle;
        

        // Get the validitiy range
        EventIDRange rangeW;
        if (!readHandle.range(rangeW)) {
            ATH_MSG_FATAL("updateFieldMap: Failed to retrieve validity range for " << readHandle.key());
            return StatusCode::FAILURE;
        }
        cache.m_mapCondObjOutputRange = rangeW;
        ATH_MSG_INFO("updateFieldMap: Update map from conditions: Range of input/output is "
                     << cache.m_mapCondObjOutputRange);

        ATH_MSG_INFO( "updateFieldMap: reading magnetic field map filename from COOL" );

        for (CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr) {
            const coral::AttributeList &attr = itr->second;
            const std::string &mapType = attr["FieldType"].data<std::string>();
            const std::string &mapFile = attr["MapFileName"].data<std::string>();
        
            ATH_MSG_INFO("updateFieldMap: found map of type " << mapType << " (path " << mapFile << ")");

            // first 5 letters are reserved (like "file:")
            const std::string mapFile_decoded = mapFile.substr(5);
            if (mapType == "GlobalMap") {
                fullMapFilename = mapFile_decoded;
                break;
            }
        }
    
    } 
    else
    {
        // not m_useMapsFromCOOL - set values from job options
        fullMapFilename = m_fullMapFilename;
    }
    // Select map file according to the value of the currents which indicate which map is 'on'

    // determine the map to load
    std::string mapFile { fullMapFilename };

    ATH_MSG_INFO ( "updateFieldMap: Use map file " << mapFile);

        
    // find the path to the map file
    std::string resolvedMapFile = PathResolver::find_file( mapFile.c_str(), "DATAPATH" );
    if ( resolvedMapFile.empty() ) {
        ATH_MSG_ERROR( "updateFieldMap: Field map file " << mapFile << " not found" );
        return StatusCode::FAILURE;
    }
    // Do checks and extract root file to initialize the map
    if ( resolvedMapFile.find(".root") == std::string::npos ) {
        ATH_MSG_ERROR("updateFieldMap: input file name '" << resolvedMapFile << "' does not end with .root");
        return StatusCode::FAILURE;
    } 
    TFile* rootfile = new TFile(resolvedMapFile.c_str(), "OLD");
    if ( ! rootfile ) {
        ATH_MSG_ERROR("updateFieldMap: failed to open " << resolvedMapFile);
        return StatusCode::FAILURE;
    }
    if ( !rootfile->cd() ) {
        // could not make it current directory
        ATH_MSG_ERROR("updateFieldMap: unable to cd() into the ROOT field map TFile");
        rootfile->Close();
        delete rootfile;
        return StatusCode::FAILURE; 
    }
    // open the tree
    TTree* tree = (TTree*)rootfile->Get("BFieldMap");
    if ( tree == nullptr ) {
        // no tree
        ATH_MSG_ERROR("updateFieldMap: TTree 'BFieldMap' does not exist in ROOT field map");
        rootfile->Close();
        delete rootfile;
        return StatusCode::FAILURE;
    }

    // create map
    cache.m_fieldMap = std::make_unique<MagField::FaserFieldMap>();

    // initialize map
    if (!cache.m_fieldMap->initializeMap( rootfile )) {
        // failed to initialize the map
        ATH_MSG_ERROR("updateFieldMap: unable to initialize the map for FaserFieldMap for file " << resolvedMapFile);
        rootfile->Close();
        delete rootfile;
        return StatusCode::FAILURE;
    }
    
    rootfile->Close();
    delete rootfile;
    

    ATH_MSG_INFO( "updateFieldMap: Initialized the field map from " << resolvedMapFile );

    return StatusCode::SUCCESS;
}
    

StatusCode
MagField::FaserFieldMapCondAlg::finalize() {
    ATH_MSG_INFO ( " in finalize " );
    return StatusCode::SUCCESS; 
}
