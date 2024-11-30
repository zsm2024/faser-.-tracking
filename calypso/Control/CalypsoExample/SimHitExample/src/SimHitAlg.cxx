#include "SimHitAlg.h"

SimHitAlg::SimHitAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthHistogramAlgorithm(name, pSvcLocator) { m_hist = nullptr; }

SimHitAlg::~SimHitAlg() { }

StatusCode SimHitAlg::initialize()
{
    // initialize a histogram 
    // letter at end of TH1 indicated variable type (D double, F float etc)
    m_hist = new TH1D("eLoss", "SCT Hit Energy Loss", 100, 0, 1); //first string is root object name, second is histogram title
    m_module = new TH2D("module", "SCT Hit Module", 3, -1.5, 1.5, 4, -0.5, 3.5 );
    m_moduleSide1 = new TH2D("moduleSide1", "SCT Hit Module", 3, -1.5, 1.5, 4, -0.5, 3.5 );
    m_moduleSide2 = new TH2D("moduleSide2", "SCT Hit Module", 3, -1.5, 1.5, 4, -0.5, 3.5 );
    ATH_CHECK(histSvc()->regHist("/HIST/eloss", m_hist));
    ATH_CHECK(histSvc()->regHist("/HIST/modules", m_module));
    ATH_CHECK(histSvc()->regHist("/HIST/modulesSide1", m_moduleSide1));
    ATH_CHECK(histSvc()->regHist("/HIST/modulesSide2", m_moduleSide2));

    m_plate_preshower = new TH2D("preshowerplate", "Preshower Hit", 3, -1, 1, 4, 0, 1 );
    m_plate_trigger = new TH2D("triggerplate", "Trigger Hit", 3, -1, 1, 4, 0, 1 );
    m_plate_veto = new TH2D("vetoplate", "Veto Hit", 3, -1, 1, 4, 0, 1 );
    ATH_CHECK(histSvc()->regHist("/HIST/plate_preshower", m_plate_preshower));
    ATH_CHECK(histSvc()->regHist("/HIST/plate_trigger", m_plate_trigger));
    ATH_CHECK(histSvc()->regHist("/HIST/plate_veto", m_plate_veto));

    m_veto_hitTime = new TH1D("vetoTime", "Veto hit time", 120, -30, 30);
    m_preshower_hitTime = new TH1D("preshowerTime", "Preshower hit time", 120, -30, 30);
    m_trigger_hitTime = new TH1D("triggerTime", "Trigger hit time", 120, -30, 30);
    m_ecal_hitTime = new TH1D("ecalTime", "Ecal hit time", 120, -30, 30);
    ATH_CHECK(histSvc()->regHist("/HIST/vetoTime", m_veto_hitTime));
    ATH_CHECK(histSvc()->regHist("/HIST/triggerTime", m_trigger_hitTime));
    ATH_CHECK(histSvc()->regHist("/HIST/preshowerTime", m_preshower_hitTime));
    ATH_CHECK(histSvc()->regHist("/HIST/ecalTime", m_ecal_hitTime));

    m_ecalEnergy = new TH1D("ecalEnergy", "Ecal Energy Fraction", 100, 0.0, 0.20);
    ATH_CHECK(histSvc()->regHist("/HIST/ecal_energy", m_ecalEnergy));

    m_emulsionPDG = new TH1D("emulsionPDG", "Energy-weighted PDG code of emulsion hits", 4425, -2212.5, 2212.5);
    ATH_CHECK(histSvc()->regHist("/HIST/emulsionPDG", m_emulsionPDG));

    // initialize data handle keys
    ATH_CHECK( m_mcEventKey.initialize() );
    ATH_CHECK( m_faserSiHitKey.initialize() );

    ATH_CHECK( m_emulsionHitKey.initialize() );

    ATH_CHECK( m_preshowerHitKey.initialize() );
    ATH_CHECK( m_triggerHitKey.initialize() );
    ATH_CHECK( m_vetoHitKey.initialize() );

    ATH_CHECK( m_ecalHitKey.initialize() );

    ATH_MSG_INFO( "Using GenEvent collection with key " << m_mcEventKey.key());
    ATH_MSG_INFO( "Using Faser SiHit collection with key " << m_faserSiHitKey.key());

    ATH_MSG_INFO( "Using Emulsion NeutrinoHit collection with key " << m_emulsionHitKey.key() );

    ATH_MSG_INFO( "Using Preshower ScintHit collection with key " << m_preshowerHitKey.key());
    ATH_MSG_INFO( "Using Trigger ScintHit collection with key " << m_triggerHitKey.key());
    ATH_MSG_INFO( "Using Veto ScintHit collection with key " << m_vetoHitKey.key());

    ATH_MSG_INFO( "Using CaloHit collection with key " << m_ecalHitKey.key());
    return StatusCode::SUCCESS;


}

StatusCode SimHitAlg::execute()
{
    // Handles created from handle keys behave like pointers to the corresponding container
    SG::ReadHandle<McEventCollection> h_mcEvents(m_mcEventKey);
    ATH_MSG_INFO("Read McEventContainer with " << h_mcEvents->size() << " events");
    if (h_mcEvents->size() == 0) return StatusCode::FAILURE;

    SG::ReadHandle<FaserSiHitCollection> h_siHits(m_faserSiHitKey);
    ATH_MSG_INFO("Read FaserSiHitCollection with " << h_siHits->size() << " hits");

    SG::ReadHandle<NeutrinoHitCollection> h_emulsionHits(m_emulsionHitKey);
    if (h_emulsionHits.isValid())
    {
        ATH_MSG_INFO("Read NeutrinoHitCollection with " << h_emulsionHits->size() << " hits");
    }

    SG::ReadHandle<ScintHitCollection> h_preshowerHits(m_preshowerHitKey);
    ATH_MSG_INFO("Read ScintHitCollection/PreshowerHits with " << h_preshowerHits->size() << " hits");
    SG::ReadHandle<ScintHitCollection> h_triggerHits(m_triggerHitKey);
    ATH_MSG_INFO("Read ScintHitCollection/TriggerHits with " << h_triggerHits->size() << " hits");
    SG::ReadHandle<ScintHitCollection> h_vetoHits(m_vetoHitKey);
    ATH_MSG_INFO("Read ScintHitCollection/VetoHits with " << h_vetoHits->size() << " hits");
    SG::ReadHandle<CaloHitCollection> h_ecalHits(m_ecalHitKey);
    ATH_MSG_INFO("Read CaloHitCollection with " << h_ecalHits->size() << " hits");

    // Since we have no pile-up, there should always be a single GenEvent in the container
    const HepMC::GenEvent* ev = (*h_mcEvents)[0];
    if (ev == nullptr) 
    {
        ATH_MSG_FATAL("GenEvent pointer is null");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Event contains " << ev->particles_size() << " truth particles" );

    double ePrimary = 0;
    if (ev->particles_size() > 0)
    {
#ifdef HEPMC3
        for (HepMC3::ConstGenParticlePtr p : ev->particles())
        {
          if (p->id() > 11000) continue;
          ePrimary += p->momentum().e() - p->momentum().m();
        }
#else
        HepMC::GenEvent::particle_const_iterator p = ev->particles_begin();
        for ( ; p !=  ev->particles_end(); ++p) {
          // Particle gun primaries have BCs starting at 10000, while others have 3 digit values
          if ((*p)->barcode() > 11000) continue;
          ePrimary += (*p)->momentum().e() - (*p)->momentum().m();
        }
#endif        
        std::cout << "Kinetic energy of primary particles: " << ePrimary << std::endl;
    } 

    if (h_emulsionHits.isValid() && h_emulsionHits->size()!=0)
    {
        //Loop over all hits; print
        for (const NeutrinoHit& hit : *h_emulsionHits)
        {
            if (m_printNeutrino) hit.print();
            if (hit.particleLink().isValid())
            {
                m_emulsionPDG->Fill(hit.particleLink()->pdg_id(), hit.energyLoss());
            }
        }
    }

    // The hit container might be empty because particles missed the wafers
    //if (h_siHits->size() == 0) return StatusCode::SUCCESS;

    if (h_siHits->size()!=0){
        // Loop over all hits; print and fill histogram
        for (const FaserSiHit& hit : *h_siHits)
        {
            if (m_printTracker) hit.print();
            m_hist->Fill( hit.energyLoss() );
            m_module->Fill( hit.getModule(), hit.getRow() );
            if (hit.getSensor() == 0)
            {
                m_moduleSide1->Fill( hit.getModule(), hit.getRow());
            }
            else
            {
                m_moduleSide2->Fill( hit.getModule(), hit.getRow());
            }

        }
    }

    if (h_preshowerHits->size()!=0){
        for (const ScintHit& hit : *h_preshowerHits)
        {
            if (m_printScintillator) hit.print();
            m_hist->Fill(hit.energyLoss());
            m_plate_preshower->Fill(hit.getStation(),hit.getPlate(),hit.energyLoss());
            m_preshower_hitTime->Fill(hit.meanTime(), hit.energyLoss());
        }
    }

    if (h_triggerHits->size()!=0){
        for (const ScintHit& hit : *h_triggerHits)
        {
            if (m_printScintillator) hit.print();
            m_hist->Fill(hit.energyLoss());
            m_plate_trigger->Fill(hit.getStation(),hit.getPlate(),hit.energyLoss());
            m_trigger_hitTime->Fill(hit.meanTime(), hit.energyLoss());
        }
    }

    if (h_vetoHits->size()!=0){
        for (const ScintHit& hit : *h_vetoHits)
        {
            if (m_printScintillator) hit.print();
            m_hist->Fill(hit.energyLoss());
            m_plate_veto->Fill(hit.getStation(),hit.getPlate(),hit.energyLoss());
            m_veto_hitTime->Fill(hit.meanTime(), hit.energyLoss());
        }
    }

    if (h_ecalHits->size() != 0)
    {
        double ecalTotal = 0.0;
        for (const CaloHit& hit : *h_ecalHits)
        {
            if (m_printCalorimeter) hit.print();
            ecalTotal += hit.energyLoss();
            m_ecal_hitTime->Fill(hit.meanTime(), hit.energyLoss());
        }
        if (ePrimary > 0) m_ecalEnergy->Fill(ecalTotal/ePrimary);
    }
    return StatusCode::SUCCESS;
}

StatusCode SimHitAlg::finalize()
{
    return StatusCode::SUCCESS;
}