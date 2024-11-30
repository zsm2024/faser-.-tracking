/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FaserTruthSvc.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef FASERISF_SERVICES_FASERTRUTHSVC_H
#define FASERISF_SERVICES_FASERTRUTHSVC_H 1

// STL includes
#include <string>

// FrameWork includes
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthService.h"

// ISF include
#include "FaserISF_Interfaces/IFaserTruthSvc.h"
#include "FaserISF_HepMC_Interfaces/IFaserTruthStrategy.h"
#include "FaserISF_Event/IFaserTruthIncident.h"

// DetectorDescription
#include "FaserDetDescr/FaserRegion.h"

// Barcode
#include "BarcodeInterfaces/IBarcodeSvc.h"
#include "BarcodeEvent/Barcode.h"

// McEventCollection
#include "GeneratorObjects/McEventCollection.h"

// forward declarations
class StoreGateSvc;

// forward declarations
#include "AtlasHepMC/GenEvent_fwd.h"
#include "AtlasHepMC/GenVertex.h"
// #include "AtlasHepMC/SimpleVector.h"

namespace ISFTesting {
  class TruthSvc_test;
}

namespace ISF {

  /** @class FaserTruthSvc

      HepMC based version of the ISF::ITruthSvc,
      currently it takes an IFaserTruthIncident base class


      @author Andreas.Salzburger -at- cern.ch , Elmar.Ritsch -at- cern.ch
  */
  class FaserTruthSvc final : public extends<AthService, IFaserTruthSvc> {

    // allow test to access private data
    friend ISFTesting::TruthSvc_test;

  public:

    //** Constructor with parameters */
    FaserTruthSvc( const std::string& name, ISvcLocator* pSvcLocator );

    /** Destructor */
    virtual ~FaserTruthSvc() = default;

    /** Athena algorithm's interface method initialize() */
    StatusCode  initialize() override;
    /** Athena algorithm's interface method finalize() */
    StatusCode  finalize() override;

    /** Register a truth incident */
    void registerTruthIncident( IFaserTruthIncident& truthincident, bool saveAllChildren = false) const override;

    /** Initialize the Truth Svc at the beginning of each event */
    StatusCode initializeTruthCollection() override;

    /** Finalize the Truth Svc at the end of each event*/
    StatusCode releaseEvent() override;

  private:
    /** Record the given truth incident to the MC Truth */
    void recordIncidentToMCTruth( IFaserTruthIncident& truthincident, bool passWholeVertex) const;
    /** Record and end vertex to the MC Truth for the parent particle */
    HepMC::GenVertexPtr createGenVertexFromTruthIncident( IFaserTruthIncident& truthincident,
                                                        bool replaceExistingGenVertex=false) const;

    /** Set shared barcode for child particles */
    void setSharedChildParticleBarcode( IFaserTruthIncident& truthincident) const;

    /** Delete child vertex */
    void deleteChildVertex(HepMC::GenVertexPtr vertex) const;

    /** Helper function to determine the largest particle barcode set by the generator */
    int maxGeneratedParticleBarcode(const HepMC::GenEvent *genEvent) const;

    /** Helper function to determine the largest vertex barcode set by the generator */
    int maxGeneratedVertexBarcode(const HepMC::GenEvent *genEvent) const;

    /** Helper function to classify existing GenVertex objects */
    ISF::InteractionClass_t interactionClassification(HepMC::GenVertexPtr& vtx) const;

    ServiceHandle<Barcode::IBarcodeSvc>       m_barcodeSvc{this, "BarcodeSvc", "BarcodeSvc", ""};            //!< The Barcode service

    /** the truth strategies applied (as AthenaToolHandle Array) */
    ToolHandleArray<IFaserTruthStrategy>      m_truthStrategies {this, "TruthStrategies", {}, ""};
    /** for faster access: using an internal pointer to the actual ITruthStrategy instances */
    IFaserTruthStrategy**                     m_geoStrategies[FaserDetDescr::fNumFaserRegions];
    unsigned short                            m_numStrategies[FaserDetDescr::fNumFaserRegions];

    /** MCTruth steering */
    Gaudi::Property<bool>                     m_skipIfNoChildren {this, "SkipIfNoChildren", true, ""};       //!< do not record incident if numChildren==0
    Gaudi::Property<bool>                     m_skipIfNoParentBarcode {this, "SkipIfNoParentBarcode", true, ""};   //!< do not record if parentBarcode==fUndefinedBarcode
    Gaudi::Property<bool>                     m_ignoreUndefinedBarcodes {this, "IgnoreUndefinedBarcodes",  false, ""};//!< do/don't abort if retrieve an undefined barcode

    Gaudi::Property<bool>                     m_passWholeVertex {this, "PassWholeVertices", true, ""};

    Gaudi::Property<std::vector<unsigned int> >  m_forceEndVtxRegionsVec{this, "ForceEndVtxInRegions", {}, ""}; //!< property containing FaserRegions for which to write end-vtx
                                                                              //   to write end-vtx
    std::array<bool, FaserDetDescr::fNumFaserRegions> m_forceEndVtx; //!< attach end vertex to all parent particles if they die


    Gaudi::Property<bool>   m_quasiStableParticlesIncluded{this, "QuasiStableParticlesIncluded", false, ""}; //!< does this job simulate quasi-stable particles.
    Gaudi::Property<bool>   m_quasiStableParticleOverwrite{this, "QuasiStableParticleOverwrite", true, ""}; //!< should the job be overwriting QS particles


  };
}


#endif //> !FASERISF_SERVICES_FASERTRUTHSVC_H
