#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "NeutrinoSimEvent/NeutrinoHitCollection.h"
#include "ScintSimEvent/ScintHitCollection.h" 
#include "FaserCaloSimEvent/CaloHitCollection.h"
#include <TH1.h>


/* SimHit reading example - Ryan Rice-Smith, Savannah Shively - UC Irvine */

class SimHitAlg : public AthHistogramAlgorithm
{
    public:
    SimHitAlg(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~SimHitAlg();

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

    private:
    TH1* m_hist;  // Example histogram
    TH2* m_module;
    TH2* m_moduleSide1;
    TH2* m_moduleSide2;

    //ScintHit Histograms
    TH2* m_plate_preshower;
    TH2* m_plate_trigger;
    TH2* m_plate_veto;
    TH1* m_veto_hitTime;
    TH1* m_preshower_hitTime;
    TH1* m_trigger_hitTime;

    // Ecal histogram
    TH1* m_ecalEnergy;
    TH1* m_ecal_hitTime;

    // Emulsion PDG_ID
    TH1* m_emulsionPDG;

    // Read handle keys for data containers
    // Any other event data can be accessed identically
    // Note the key names ("BeamTruthEvent" or "SCT_Hits") are Gaudi properties and can be configured at run-time

    // Truth information
    SG::ReadHandleKey<McEventCollection> m_mcEventKey       { this, "McEventCollection", "BeamTruthEvent" };

    // Tracker hits
    SG::ReadHandleKey<FaserSiHitCollection> m_faserSiHitKey { this, "FaserSiHitCollection", "SCT_Hits" };

    // Emulsion hits
    SG::ReadHandleKey<NeutrinoHitCollection> m_emulsionHitKey { this, "NeutrinoHitCollection", "EmulsionHits" };

    //PreshowerHits, TriggerHits, VetoHits Sav new stuff
    SG::ReadHandleKey<ScintHitCollection> m_preshowerHitKey { this, "PreshowerHitCollection", "PreshowerHits" };
    SG::ReadHandleKey<ScintHitCollection> m_triggerHitKey { this, "TriggerHitCollection", "TriggerHits" };
    SG::ReadHandleKey<ScintHitCollection> m_vetoHitKey { this, "VetoHitCollection", "VetoHits" };

    //EcalHits
    SG::ReadHandleKey<CaloHitCollection> m_ecalHitKey { this, "CaloHitCollection", "EcalHits" };

    BooleanProperty m_printNeutrino { this, "PrintNeutrinoHits", false };
    BooleanProperty m_printTracker  { this, "PrintTrackerHits", false };
    BooleanProperty m_printScintillator { this, "PrintScintillatorHits", false };
    BooleanProperty m_printCalorimeter { this, "PrintCalorimeterHits", false };
};