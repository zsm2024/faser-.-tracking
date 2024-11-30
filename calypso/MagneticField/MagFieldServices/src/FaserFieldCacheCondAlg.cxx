/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class FaserFieldCacheCondAlg
   @authors RD Schaffer <r.d.schaffer -at- cern.ch>, C Anastopoulos
   @brief create a MagField object for tracking clients

   Purpose: for each event, create a MagField object for tracking clients and store in conditions
   Store

**/

// ISF_Services include
#include "FaserFieldCacheCondAlg.h"

// Concurrency
#include "GaudiKernel/ConcurrencyFlags.h"

// PathResolver
#include "PathResolver/PathResolver.h"

// ROOT
#include "TFile.h"
#include "TTree.h"

MagField::FaserFieldCacheCondAlg::FaserFieldCacheCondAlg(const std::string& name, 
                                                   ISvcLocator* pSvcLocator)
    :AthReentrantAlgorithm(name, pSvcLocator){ 
}

MagField::FaserFieldCacheCondAlg::~FaserFieldCacheCondAlg(){ }

StatusCode
MagField::FaserFieldCacheCondAlg::initialize() {

    ATH_MSG_INFO ("Initialize");
    // CondSvc
    ATH_CHECK( m_condSvc.retrieve() );

    // Read handle for the field scale factors conditions object
    ATH_CHECK( m_scaleInputKey.initialize() );

    // Read handle for the field map cond object
    ATH_CHECK( m_mapCondObjInputKey.initialize() );

    // Output handle for scale factors/cache
    ATH_CHECK( m_condObjOutputKey.initialize() );

    // Register write handle for scale factors/cache
    if (m_condSvc->regHandle(this, m_condObjOutputKey).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle " << m_condObjOutputKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ( "Initialize: Key " << m_condObjOutputKey.fullKey() << " has been succesfully registered " );
    
    return StatusCode::SUCCESS;
}

StatusCode
MagField::FaserFieldCacheCondAlg::execute(const EventContext& ctx) const {

    ATH_MSG_DEBUG ( "execute: entering");

    // Check if output conditions object with field cache object is still valid, if not replace it
    // with new current scale factors
    SG::WriteCondHandle<FaserFieldCacheCondObj> writeHandle{m_condObjOutputKey, ctx};
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG("execute: CondHandle " << writeHandle.fullKey() 
                      << " is already valid. " 
                      <<" May happen if multiple concurrent events are being processed out of order.");
        return StatusCode::SUCCESS;
    }  

    //This will need to be filled before we construct the condition object 
    Cache cache{};

    // set current scale factor from either conditions or from jobOption parameters
    if (m_useDCS) {
        ATH_CHECK( updateScaleFromConditions(ctx, cache) );
    }
    else {
        ATH_CHECK( updateScaleFromParameters(cache) );
    }

    // Must read map cond object to get previously created map
    SG::ReadCondHandle<FaserFieldMapCondObj> mapReadHandle{m_mapCondObjInputKey, ctx};
    const FaserFieldMapCondObj* mapCondObj{*mapReadHandle};
    if (mapCondObj == nullptr) {
        ATH_MSG_ERROR("execute: Could not access conditions map for key: " << m_mapCondObjInputKey.fullKey());
        return StatusCode::FAILURE;
    }

    // simple pointer to the map, to be give to the AtlasFieldCacheCondObj, used for the cache field map access
    const MagField::FaserFieldMap* fieldMap =  mapCondObj->fieldMap();

    // save current scale factor in conditions object
    auto fieldCondObj = std::make_unique<FaserFieldCacheCondObj>();

    // initialize cond obj with current scale factors and the field svc (needed to setup cache)
    if (!fieldCondObj->initialize(cache.m_dipScaleFactor, 
                                  fieldMap)) {
        ATH_MSG_ERROR("execute: Could not initialize conditions field object with field map "
                      << cache.m_dipScaleFactor );
        return StatusCode::FAILURE;
    }

    // Record in conditions store the conditions object with scale factors and map pointer for cache
    if(writeHandle.record(cache.m_condObjOutputRange, std::move(fieldCondObj)).isFailure()) {
        ATH_MSG_ERROR("execute: Could not record FaserFieldCacheCondObj object with " 
                      << writeHandle.key() << " with EventRange " << cache.m_condObjOutputRange
                      << " into Conditions Store");
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO ( "execute: initialized FaserFieldCacheCondObj and cache with scale factor "
                   << cache.m_dipScaleFactor );
    ATH_MSG_INFO ( "execute: dipole zone id  " << fieldMap->dipoleZoneId());

    return StatusCode::SUCCESS;
}

StatusCode
MagField::FaserFieldCacheCondAlg::updateScaleFromConditions(const EventContext& ctx, Cache& cache) const
{
    ATH_MSG_INFO ( "UpdateScaleFromConditions  " );

    // readin current value
    SG::ReadCondHandle<CondAttrListCollection> readHandle {m_scaleInputKey, ctx};
    const CondAttrListCollection* attrListColl{*readHandle};
    if (attrListColl == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve CondAttributeListCollection with key " << m_scaleInputKey.key());
        return StatusCode::FAILURE;
    }

    // Get the validitiy range
    EventIDRange rangeW;
    if (!readHandle.range(rangeW)) {
        ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
        return StatusCode::FAILURE;
    }
    cache.m_condObjOutputRange = rangeW;
    ATH_MSG_INFO("Range of input/output is " <<  cache.m_condObjOutputRange);

    bool   gotscale{false};
    double dipscale {0.};

    // Channel names don't seem to be read even when they are present...
    bool hasChanNames {false};
    for ( CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr ) {
        const std::string& name = attrListColl->chanName(itr->first);
        ATH_MSG_INFO( "Trying to read from DCS: [channel name, index, value] "
                      << name << " , " << itr->first << " , " << itr->second["value"].data<float>() );
        if (name.compare("") != 0) {
            hasChanNames = true;
        }
        if ( name.compare("Dipole_Scale") == 0 ) {
            dipscale = itr->second["value"].data<float>();
            gotscale = true;
        }
    }
    if ( !hasChanNames ) {
        ATH_MSG_INFO( "UpdateCurrentFromConditions: Attempt 2 at reading currents from DCS (using channel index)" );
        // in no channel is named, try again using channel index instead
        for ( CondAttrListCollection::const_iterator itr = attrListColl->begin(); itr != attrListColl->end(); ++itr ) {

            if ( itr->first == 1 ) {
                // channel 1 is dipole scale
                dipscale = itr->second["value"].data<float>();
                gotscale = true;
            }
        }
    }

    if ( !gotscale ) {
        ATH_MSG_ERROR( "Missing dipole scale factor in DCS information" );
        return StatusCode::FAILURE;
    }

    if (dipscale < m_dipoMinScale)
    {
        dipscale = 0.0;
        ATH_MSG_INFO("Dipole is off.");
    }

    cache.m_dipScaleFactor = dipscale;
    return StatusCode::SUCCESS;
}

StatusCode
MagField::FaserFieldCacheCondAlg::updateScaleFromParameters(Cache& cache) const
{

    ATH_MSG_INFO( "updateCurrentFromParameters" );
    // take the current value from JobOptions
    double dipscale{m_useDipoScale};
    if ( dipscale < m_dipoMinScale ) {
        dipscale = 0.0;
        ATH_MSG_INFO( "Dipole is off" );
    }
    cache.m_dipScaleFactor = dipscale;
    ATH_MSG_INFO("Update from job options: Range of input/output is " <<  cache.m_condObjOutputRange);
    ATH_MSG_INFO("Magnetic field scale taken from jobOption parameters " );
    return StatusCode::SUCCESS;
}

StatusCode
MagField::FaserFieldCacheCondAlg::finalize() {
    ATH_MSG_INFO ( " in finalize " );
    return StatusCode::SUCCESS; 
}
