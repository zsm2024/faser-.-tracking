#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "GeneratorObjects/McEventCollection.h"
#include "TrackerSimEvent/FaserSiHitCollection.h"
#include "NeutrinoSimEvent/NeutrinoHitCollection.h"
#include "ScintSimEvent/ScintHitCollection.h" 
#include "FaserCaloSimEvent/CaloHitCollection.h"
#include <TH1.h>


/* SimHit reading example - Ryan Rice-Smith, Savannah Shively - UC Irvine */

class EmulsionID;
namespace NeutrinoDD {
class EmulsionDetectorManager;
}


class NeutrinoRecAlgs : public AthHistogramAlgorithm
{
    public:
    NeutrinoRecAlgs(const std::string& name, ISvcLocator* pSvcLocator);

    virtual ~NeutrinoRecAlgs();

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

    TH1* m_ecalEnergy;

    int m_runnumber;
    int m_event_id;
    int m_track_id_MC;
    int m_track_id_hit;
    int m_track_id;

    //For emulsion hit
    TTree* m_NuHit_tree;

    int   m_pdg_MC;
    float m_x_start;
    float m_y_start;
    float m_z_start;

    float m_x_end;
    float m_y_end;
    float m_z_end;

    float m_vx_prod_hit;
    float m_vy_prod_hit;
    float m_vz_prod_hit;

    float m_vx_decay_hit;
    float m_vy_decay_hit;
    float m_vz_decay_hit;

    float m_x_start_global;
    float m_y_start_global;
    float m_z_start_global;

    float m_x_end_global;
    float m_y_end_global;
    float m_z_end_global;

    float m_slope_x_global;
    float m_slope_y_global;
    float m_slope_z_global;

    float m_emeloss;
    float m_slope_x;
    float m_slope_y;
    float m_slope_z;

    int m_plate;
    int m_base;
    int m_emmodule;
    int m_film;

    double m_px_MC;
    double m_py_MC;
    double m_pz_MC;

    double m_energy_MC;
    double m_kinetic_energy_MC;
    double m_mass_MC;

    int m_num_in_particle;
    int m_num_out_particle;

    int m_trackid_begin_in_particle;
    int m_trackid_begin_out_particle;
    int m_trackid_end_in_particle;
    int m_trackid_end_out_particle;

    int m_status;
    
    std::vector<int> m_pdg_in_particle;
    std::vector<int> m_pdg_out_particle;
    std::vector<int> m_trackid_in_particle;
    std::vector<int> m_trackid_out_particle; 
    std::vector<int> m_status_in_particle;
    std::vector<int> m_status_out_particle; 
    
    HepMcParticleLink m_trackLink;

    //For truth info
    TTree* m_NuMCTruth_tree;
    float m_vx_prod;
    float m_vy_prod;
    float m_vz_prod;

    float m_vx_decay;
    float m_vy_decay;
    float m_vz_decay;

    int m_event_id_MC;
    int m_pdg_id;
    double m_px;
    double m_py;
    double m_pz;

    double m_energy;
    double m_kinetic_energy;
    double m_mass;
    
    const NeutrinoDD::EmulsionDetectorManager  *m_emulsiom;
    const EmulsionID *m_sID;


    // Read handle keys for data containers
    // Any other event data can be accessed identically
    // Note the key names ("BeamTruthEvent" or "SCT_Hits") are Gaudi properties and can be configured at run-time

    // Truth information
    SG::ReadHandleKey<McEventCollection> m_mcEventKey       { this, "McEventCollection", "TruthEvent" };

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
};
