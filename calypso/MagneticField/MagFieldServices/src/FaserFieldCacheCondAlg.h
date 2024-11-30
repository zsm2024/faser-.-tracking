/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


///////////////////////////////////////////////////////////////////
// FaserFieldCacheCondAlg.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MAGFIELDSERVICES_FASERFIELDCACHECONDALG_H
#define MAGFIELDSERVICES_FASERFIELDCACHECONDALG_H 

// FrameWork includes
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"

// #include "MagFieldInterfaces/IMTMagFieldSvc.h"

#include "MagFieldConditions/FaserFieldCacheCondObj.h"
#include "MagFieldConditions/FaserFieldMapCondObj.h"

namespace MagField {

    class FaserFieldCacheCondAlg : public AthReentrantAlgorithm 
    {
    public:
        FaserFieldCacheCondAlg(const std::string& name,ISvcLocator* pSvcLocator);
        virtual ~FaserFieldCacheCondAlg();

        StatusCode initialize() override final;
        StatusCode execute(const EventContext& ctx) const override final;
        StatusCode finalize() override final;  

    private:

        /*
         *  Cache of the variables to be updated before we write the conditions object
         */
        struct Cache{
            // float  m_solenoidCurrent{0};   // solenoid current in ampere
            // float  m_toroidCurrent{0};     // toroid current in ampere
            // double m_solScaleFactor{1};    // solenoid current scale factor
            // double m_torScaleFactor{1};    // toroid current scale factor
            double m_dipScaleFactor{1};
            //"infinite in case we do not update from DCS" - full run/event range
            EventIDRange m_condObjOutputRange {EventIDRange(EventIDBase(0,0), EventIDBase(EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFEVT-1))}; 
        }; 
        StatusCode updateScaleFromConditions(const EventContext& ctx, Cache& cache) const;
        StatusCode updateScaleFromParameters(Cache& cache) const;

        // threshold below which currents are considered zero
        Gaudi::Property<double> m_dipoMinScale {this, 
                                                  "DipoMinScale", 0.01, "Minimum dipole scale factor for which dipole is considered ON"};

        // flag to use magnet current from DCS in COOL
        Gaudi::Property<bool> m_useDCS {this, 
                                        "UseDCS", false, "Get magnet scale factor from DCS through ConditionsSvc"};

        // COOL folder name containing current information
        // current input key
        SG::ReadCondHandleKey<CondAttrListCollection> m_scaleInputKey
        {this, 
         "COOLCurrentsFolderName", "/GLOBAL/BField/Scales", "Name of the COOL folder containing magnet scale factor"};

        // FaserFieldMapCondObj - read handle to access magnet field conditions object containing the map file names
        SG::ReadCondHandleKey<FaserFieldMapCondObj> m_mapCondObjInputKey
        {this, 
         "FaserFieldMapCondObj", "fieldMapCondObj", "Name of key for the Magnetic Field conditions object with the map file names"};

        // FaserFieldCacheCondObj - magnet field conditions object containing the current scale factors
        SG::WriteCondHandleKey<FaserFieldCacheCondObj> m_condObjOutputKey
        {this, 
         "FaserFieldCacheCondObj", "fieldCondObj", "Name of the key for the Magnetic Field conditions object with currents for scaling"};

        Gaudi::Property<double> m_useDipoScale {this, 
                                                  "UseDipoScale", 1.0,  "Set actual dipole scale factor"};

        ServiceHandle<ICondSvc> m_condSvc { this, 
                                            "CondSvc", "CondSvc", "conditions service" };

    };
}

#endif //> !MAGFIELDSERVICES_FASERFIELDCACHECONDALG_H
