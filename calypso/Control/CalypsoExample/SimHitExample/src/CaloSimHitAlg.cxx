#include "CaloSimHitAlg.h"



CaloSimHitAlg::CaloSimHitAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthHistogramAlgorithm(name, pSvcLocator) { m_eloss = nullptr; }

CaloSimHitAlg::~CaloSimHitAlg() { }

StatusCode CaloSimHitAlg::initialize()

{ 

    // initialize a histogram 
    // letter at end of TH1 indicated variable type (D double, F float etc)
    // first string is root object name, second is histogram title
 

  m_eloss = new TH1D("eLoss", "Calo Hit Energy Loss;Energy Loss [GeV];Events", 1000000, 0, 100); // 100000 bins, can rebin if needed - 1000 GeV inj - max 100 per event 
    m_elossTL = new TH1D("eLossTL", "Calo Hit Energy Loss Top Left Module;Energy Loss [GeV];Events", 100000, 0, 100);
    m_elossTR = new TH1D("eLossTR", "Calo Hit Energy Loss Top Right Module;Energy Loss [GeV];Events", 100000, 0, 100);
    m_elossBR = new TH1D("eLossBR", "Calo Hit Energy Loss Bottom Right Module;Energy Loss [GeV];Events", 100000, 0, 100);
    m_elossBL = new TH1D("eLossBL", "Calo Hit Energy Loss Bottom Left Module;Energy Loss [GeV];Events", 100000, 0, 100);
// 1 million bins
    m_elossTot = new TH1D("eLossTot", "Total Energy Deposited in Calorimeter;Deposited Energy [GeV];Events", 1000000, 0, 1000);   
    m_elossTLTot = new TH1D("eLossTLTot", "Total Energy Deposited in Top Left Module;Deposited Energy [GeV];Events", 1000000, 0, 1000);
    m_elossTRTot = new TH1D("eLossTRTot", "Total Energy Deposited in Top Right Module;Deposited Energy [GeV];Events", 1000000, 0, 1000);
    m_elossBRTot = new TH1D("eLossBRTot", "Total Energy Deposited in Bottom Right Module;Deposited Energy [GeV];Events", 1000000, 0, 1000);
    m_elossBLTot = new TH1D("eLossBLTot", "Total Energy Deposited in Bottom Left Module;Deposited Energy [GeV];Events", 1000000, 0, 1000);    

    m_module = new TH2D("module;x;y", "Calo Hit Module", 2, -0.5, 1.5, 2, -0.5, 1.5 );
    // m_modulePos = new TH2D("modulePos;x;y", "Calo Hit Module Position", 13, -130, 130, 13, -130, 130 );
    // x4 original number of bins
    m_modulePos = new TH2D("modulePos;x;y", "Calo Hit Module Position", 52, -130, 130, 52, -130, 130 );

    // as a fraction of injected energy (when known)

    m_eloss_frac = new TH1D("eLoss_frac", "Calo Hit Energy Loss Fraction;Fraction;Events", 100000, 0, 0.1); 
    m_elossTL_frac = new TH1D("eLossTL_frac", "Calo Hit Energy Loss Fraction Top Left Module;Fraction;Events", 100000, 0, 0.01);
    m_elossTR_frac = new TH1D("eLossTR_frac", "Calo Hit Energy Loss Fraction Top Right Module;Fraction;Events", 100000, 0, 0.01);
    m_elossBR_frac = new TH1D("eLossBR_frac", "Calo Hit Energy Loss Fraction Bottom Right Module;Fraction;Events", 100000, 0, 0.01);
    m_elossBL_frac = new TH1D("eLossBL_frac", "Calo Hit Energy Loss Fraction Bottom Left Module;Fraction;Events", 100000, 0, 0.01);

    m_elossTot_frac = new TH1D("eLossTot_frac", "Total Energy Fraction Deposited in Calorimeter;Fraction;Events", 1000000, 0, 1);   
    m_elossTLTot_frac = new TH1D("eLossTLTot_frac", "Total Energy Fraction Deposited in Top Left Module;Fraction;Events", 1000000, 0, 1);
    m_elossTRTot_frac = new TH1D("eLossTRTot_frac", "Total Energy Fraction Deposited in Top Right Module;Fraction;Events", 1000000, 0, 1);
    m_elossBRTot_frac = new TH1D("eLossBRTot_frac", "Total Energy Fraction Deposited in Bottom Right Module;Fraction;Events", 1000000, 0, 1);
    m_elossBLTot_frac = new TH1D("eLossBLTot_frac", "Total Energy Fraction Deposited in Bottom Left Module;Fraction;Events", 1000000, 0, 1);  

    // new histograms hits vs time 29/6/21

    m_meanTime = new TH1D("meanTime", "Events vs Mean Time;Time [ns];Events",4000, 0, 2);
    m_weightedTime = new TH1D("weightedTime", "Weighted Events vs Time;Time [ns];Events",4000, 0, 2); // weighted energy should be in GeV

    ATH_CHECK(histSvc()->regHist("/HIST/meanTime", m_meanTime));
    ATH_CHECK(histSvc()->regHist("/HIST/weightedTime", m_weightedTime));     

    ATH_CHECK(histSvc()->regHist("/HIST/eloss", m_eloss));
    ATH_CHECK(histSvc()->regHist("/HIST/elossTL", m_elossTL)); 
    ATH_CHECK(histSvc()->regHist("/HIST/elossTR", m_elossTR));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBR", m_elossBR));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBL", m_elossBL));

    ATH_CHECK(histSvc()->regHist("/HIST/elossTot", m_elossTot));
    ATH_CHECK(histSvc()->regHist("/HIST/elossTLTot", m_elossTLTot));
    ATH_CHECK(histSvc()->regHist("/HIST/elossTRTot", m_elossTRTot));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBRTot", m_elossBRTot));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBLTot", m_elossBLTot));
 
    ATH_CHECK(histSvc()->regHist("/HIST/modules", m_module));
    ATH_CHECK(histSvc()->regHist("/HIST/modulePos", m_modulePos));

    // as a fraction of injected energy 

    ATH_CHECK(histSvc()->regHist("/HIST/eloss_frac", m_eloss_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossTL_frac", m_elossTL_frac)); 
    ATH_CHECK(histSvc()->regHist("/HIST/elossTR_frac", m_elossTR_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBR_frac", m_elossBR_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBL_frac", m_elossBL_frac));

    ATH_CHECK(histSvc()->regHist("/HIST/elossTot_frac", m_elossTot_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossTLTot_frac", m_elossTLTot_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossTRTot_frac", m_elossTRTot_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBRTot_frac", m_elossBRTot_frac));
    ATH_CHECK(histSvc()->regHist("/HIST/elossBLTot_frac", m_elossBLTot_frac));

    // initialize data handle keys
    ATH_CHECK( m_mcEventKey.initialize() );
    ATH_CHECK( m_faserCaloHitKey.initialize() );
    ATH_MSG_INFO( "Using GenEvent collection with key " << m_mcEventKey.key());
    ATH_MSG_INFO( "Using Faser CaloHit collection with key " << m_faserCaloHitKey.key());

    return StatusCode::SUCCESS;
}

StatusCode CaloSimHitAlg::execute()
{
    // Handles created from handle keys behave like pointers to the corresponding container
    SG::ReadHandle<McEventCollection> h_mcEvents(m_mcEventKey);
    ATH_MSG_INFO("Read McEventContainer with " << h_mcEvents->size() << " events");
    if (h_mcEvents->size() == 0) return StatusCode::FAILURE;

    SG::ReadHandle<CaloHitCollection> h_caloHits(m_faserCaloHitKey);
    ATH_MSG_INFO("Read CaloHitCollection with " << h_caloHits->size() << " hits");

    // Since we have no pile-up, there should always be a single GenEvent in the container
    #ifdef HEPMC3
    const HepMC3::GenEvent* ev = (*h_mcEvents)[0];
    #else
    const HepMC::GenEvent* ev = (*h_mcEvents)[0];
    #endif
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
          ePrimary += p->momentum().e();
        }
#else
        HepMC::GenEvent::particle_const_iterator p = ev->particles_begin();
        for ( ; p !=  ev->particles_end(); ++p) {
          // Particle gun primaries have BCs starting at 10000, while others have 3 digit values
          if ((*p)->barcode() > 11000) continue;
          ePrimary += (*p)->momentum().e();
        }
#endif        
        std::cout << "E Primary " << ePrimary << std::endl;
    } 
    
    //ev->print();
    //ePrimary = 1000000;

    // The hit container might be empty because particles missed the modules
    if (h_caloHits->size() == 0) return StatusCode::SUCCESS;
    
    // Loop over all hits; print and fill histogram

    // TL = top left calorimeter module
    // TR = top right calorimeter module
    // BL = bottom left calorimeter module
    // BR = bottom right calorimeter module 

    float e(0);
    float tl(0);
    float tr(0);
    float br(0);
    float bl(0);

    float e_frac(0);
    float tl_frac(0);
    float tr_frac(0);
    float br_frac(0);
    float bl_frac(0);

  

    for (const CaloHit& hit : *h_caloHits)  {
      //hit.print();
      float ehit_frac = hit.energyLoss()/ePrimary;
      float ehit = hit.energyLoss()/1000; // to convert from MeV to GeV
      m_eloss_frac->Fill(ehit_frac);
      m_eloss->Fill(ehit);
      e_frac += ehit_frac;
      e += ehit;

      // add extra time hist
      float time = hit.meanTime();
      m_meanTime->Fill(time);

      // add weighted energy hist
      m_weightedTime->Fill(time,ehit);

      //

      m_module->Fill(hit.getModule(), hit.getRow());	
      m_modulePos->Fill(hit.getModule()==0 ? -121.9/2 + hit.localStartPosition().x() : 121.9/2 + hit.localStartPosition().x(), hit.getRow()==0 ? -121.9/2 + hit.localStartPosition().y() : 121.9/2 + hit.localStartPosition().y());

	
      if (hit.getModule() == 0 && hit.getRow() == 1) // 0 1 TL
	{
	  m_elossTL->Fill(ehit);
	  m_elossTL_frac->Fill(ehit_frac);
	  tl_frac += ehit_frac;
	  tl += ehit;
	}
      if (hit.getModule() == 1 && hit.getRow() == 1) // 1 1 TR
	{
	  m_elossTR->Fill(ehit);
	  m_elossTR_frac->Fill(ehit_frac);
	  tr_frac += ehit_frac;
	  tr += ehit;
	}
      if (hit.getModule() == 1 && hit.getRow() == 0) // 1 0 BR
	{
	  m_elossBR->Fill(ehit);
	  m_elossBR_frac->Fill(ehit_frac);
	  br_frac += ehit_frac;
	  br += ehit;
	}
      if (hit.getModule() == 0 && hit.getRow() == 0) // 0 0 BL
	{
	  m_elossBL->Fill(ehit);
	  m_elossBL_frac->Fill(ehit_frac);
	  bl_frac += ehit_frac;
	  bl += ehit;
	}

    }

    m_elossTot->Fill(e);
    m_elossTLTot->Fill(tl);
    m_elossTRTot->Fill(tr);
    m_elossBRTot->Fill(br);
    m_elossBLTot->Fill(bl); 

    // as a fraction of injected energy 

    m_elossTot_frac->Fill(e_frac);
    m_elossTLTot_frac->Fill(tl_frac);
    m_elossTRTot_frac->Fill(tr_frac);
    m_elossBRTot_frac->Fill(br_frac);
    m_elossBLTot_frac->Fill(bl_frac);

    return StatusCode::SUCCESS;
}

StatusCode CaloSimHitAlg::finalize()
{
    return StatusCode::SUCCESS;
}
