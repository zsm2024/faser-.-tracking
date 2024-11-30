/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// FaserFieldMapCondAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDSERVICES_FASERFIELDMAPCONDALG_H
#define MAGFIELDSERVICES_FASERFIELDMAPCONDALG_H 

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"

// #include "MagFieldInterfaces/IMTMagFieldSvc.h"

#include "MagFieldConditions/FaserFieldMapCondObj.h"

namespace MagField {

    class FaserFieldMapCondAlg : public AthReentrantAlgorithm 
    {
    public:
        FaserFieldMapCondAlg(const std::string& name,ISvcLocator* pSvcLocator);
        virtual ~FaserFieldMapCondAlg();

        StatusCode initialize() override final;
        StatusCode execute(const EventContext& ctx) const override final;
        StatusCode finalize() override final;  

    private:

        /*
         *  Cache of the variables to be updated before we write the conditions object
         */
        struct Cache{
            // field map names
            std::string m_fullMapFilename{"data/FaserFieldTable.root"}; // all magnets on
            // field map - pointer and event id range
            std::unique_ptr<MagField::FaserFieldMap> m_fieldMap;
            
            //"infinite in case we do not update from COOL" 
            EventIDRange m_mapCondObjOutputRange {EventIDRange()}; // default range covers everything (run/event and timestamp)
        }; 

        StatusCode updateFieldMap(const EventContext& ctx, Cache& cache) const;

        /// map file names - if not read from cool
        Gaudi::Property<std::string> m_fullMapFilename {this,
                                                        "FullMapFile", "data/FaserFieldTable.root",
                                                        "File storing the full magnetic field map"};

        // flag to read magnet map filenames from COOL
        Gaudi::Property<bool> m_useMapsFromCOOL {this,
                                                 "UseMapsFromCOOL", true , "Get magnetic field map filenames from COOL"};

        // COOL folder name containing field maps
        // map input key
        SG::ReadCondHandleKey<CondAttrListCollection> m_mapsInputKey
        {this,
         "COOLMapsFolderName", "/GLOBAL/BField/Maps", "Name of the COOL folder containing field maps"};

        // FaserFieldMapCondObj - magnet field conditions object containing the map file names
        SG::WriteCondHandleKey<FaserFieldMapCondObj> m_mapCondObjOutputKey
        {this, 
         "FaserFieldMapCondObj", "fieldMapCondObj", "Name of key for the Magnetic Field conditions object with the map file names"};

        ServiceHandle<ICondSvc> m_condSvc { this, 
                                            "CondSvc", "CondSvc", "conditions service" };

    };
}

#endif //> !MAGFIELDSERVICES_FASERFIELDMAPCONDALG_H
