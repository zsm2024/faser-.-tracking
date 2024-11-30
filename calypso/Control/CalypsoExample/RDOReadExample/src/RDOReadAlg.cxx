#include "RDOReadAlg.h"
#include "TrackerSimEvent/FaserSiHitIdHelper.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrackerIdentifier/FaserSCT_ID.h"
#ifdef HEPMC3
#include "HepMC3/PrintStreams.h"
#include "HepMC3/Print.h"
#endif


RDOReadAlg::RDOReadAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthHistogramAlgorithm(name, pSvcLocator) { m_hist = nullptr; }

RDOReadAlg::~RDOReadAlg() { }

StatusCode RDOReadAlg::initialize()
{
    // initialize a histogram 
    // letter at end of TH1 indicated variable type (D double, F float etc)
    m_hist = new TH1D("GroupSize", "RDO Group Size", 8, 0, 8); //first string is root object name, second is histogram title
    m_hprof = new TProfile("IncAngleGroup", "Mean Group Size vs Incident Angle", 10, -.1, .1 ,0,5);
    m_incAnglHist = new TH1D("IncAngleHist", "Incident Angle Count", 10, -.1, .1);
    ATH_CHECK(histSvc()->regHist("/HIST/myhist", m_hist));
    ATH_CHECK(histSvc()->regHist("/HIST/myhistprof", m_hprof));
    ATH_CHECK(histSvc()->regHist("/HIST/myhistAngl", m_incAnglHist));

    // initialize data handle keys
    ATH_CHECK( m_mcEventKey.initialize() );
    ATH_CHECK( m_faserSiHitKey.initialize() );
    ATH_CHECK( m_faserRdoKey.initialize());
    ATH_CHECK( m_sctMap.initialize());
    ATH_MSG_INFO( "Using GenEvent collection with key " << m_mcEventKey.key());
    ATH_MSG_INFO( "Using Faser SiHit collection with key " << m_faserSiHitKey.key());
    ATH_MSG_INFO( "Using FaserSCT RDO Container with key " << m_faserRdoKey.key());//works
    ATH_MSG_INFO( "Using SCT_SDO_Map with key "<< m_sctMap.key());
    return StatusCode::SUCCESS;
}

StatusCode RDOReadAlg::execute()
{
    // Handles created from handle keys behave like pointers to the corresponding container
    SG::ReadHandle<McEventCollection> h_mcEvents(m_mcEventKey);
    ATH_MSG_INFO("Read McEventContainer with " << h_mcEvents->size() << " events");
    if (h_mcEvents->size() == 0) return StatusCode::FAILURE;
    // h_mcEvents->at(0)->print( msg().stream() );
    SG::ReadHandle<FaserSiHitCollection> h_siHits(m_faserSiHitKey);
    ATH_MSG_INFO("Read FaserSiHitCollection with " << h_siHits->size() << " hits");

    SG::ReadHandle<FaserSCT_RDO_Container> h_sctRDO(m_faserRdoKey);
    SG::ReadHandle<TrackerSimDataCollection> h_collectionMap(m_sctMap);

    //Looping through RDO's
    for( const auto& collection : *h_sctRDO)
    {
        for(const auto& rawdata : *collection)
        {
            //Each RDO has an identifier connecting it to the simulation data that made it
            //Allows access of the simulation data that made that RDO
            const auto identifier = rawdata->identify();
            ATH_MSG_INFO("map size "<<h_collectionMap->size());
            if( h_collectionMap->count(identifier) == 0)
            {
                ATH_MSG_INFO("no map found w/identifier "<<identifier);
                continue;
            }

            //Collection map takes identifier and returns simulation data
            const auto& simdata = h_collectionMap->find(rawdata->identify())->second;
            const auto& deposits = simdata.getdeposits();
//            ATH_MSG_INFO("deposits size "<<deposits.size());

//            ATH_MSG_INFO("identifier: "<<rawdata->identify());
//            ATH_MSG_INFO("group size of "<<rawdata->getGroupSize());
            m_hist->Fill(rawdata->getGroupSize());

            //loop through deposits to find one w/ highest energy & get barcode
            float highestDep = 0;
            int barcode = 0;
            HepMcParticleLink primary{};
            for( const auto& depositPair : deposits)
            {
                if( depositPair.second > highestDep)
                {
                    highestDep = depositPair.second;
                    primary = depositPair.first;
#ifdef HEPMC3
                    HepMC3::Print::line(primary);
                    barcode = primary->id();
                    ATH_MSG_INFO("pdg id "<<primary->pid());
#else
                    primary->print(std::cout);
                    barcode = primary->barcode();
                    ATH_MSG_INFO("pdg id "<<primary->pdg_id());
#endif                    
                }
            }
            ATH_MSG_INFO("final barcode of: " << barcode << " with energy " << primary->momentum().e());
            if (primary->end_vertex() != nullptr)
            {
#ifdef HEPMC3
                for (auto daughter : primary->end_vertex()->particles_out())
                {
                    if (daughter->id() %1000000 == primary->id())
                        ATH_MSG_INFO("    daughter barcode: " << daughter->id() << " with energy " << daughter->momentum().e());
                }
#else
                // for (auto daughter : primary->particles_out())
                for ( HepMC::GenVertex::particle_iterator partout= primary->end_vertex()->particles_begin(HepMC::children);
                        partout != particle->end_vertex()->particles_end(HepMC::children);
                        ++partout )
                {
                    const HepMC::GenParticle* daughter = *partout;
                    // TODO: Check that daughter->production_vertex() and daughter->end_vertex() exist, and bracket the point you're interested in
                    if(daughter->barcode() % 1000000 == primary->barcode())
                        ATH_MSG_INFO("    daughter barcode: " << daughter->barcode() << " with energy " << daughter->momentum().e());
                }
#endif                
            }

            //Helper function to get hit location information from RDO identifier
            const FaserSCT_ID* pix;
            ServiceHandle<StoreGateSvc> detStore ("DetectorStore", "SiHitIdHelper");
            if (detStore.retrieve().isSuccess()) {
                if (detStore->retrieve(pix, "FaserSCT_ID").isFailure()) { pix = 0; }
            }
            int station = pix->station(identifier);
            int plane = pix->layer(identifier);
            int row = pix->phi_module(identifier);
            int module = pix->eta_module(identifier);
            int sensor = pix->side(identifier);

            ATH_MSG_INFO("trying to match hit to stat/plane/row/mod/sens: "<<station<<" "<<plane<<" "<<row<<" "<<module<<" "<<sensor);
            for (const FaserSiHit& hit : *h_siHits)
            {
//                ATH_MSG_INFO("hit w/vals "<<hit.getStation()<<" "<<hit.getPlane()<<" "<<hit.getRow()<<" "<<hit.getModule()<<" "<<hit.getSensor()<<" barcode: "<<hit.trackNumber());
                //set of conditions to confirm looking at same particle in same place for SiHit as RDO
                if(hit.getStation() == station 
                    && hit.getPlane() == plane
                    && hit.getRow() == row
                    && hit.getModule() == module
                    && hit.getSensor() == sensor
                    && hit.trackNumber() == barcode)
                {
                    ATH_MSG_INFO("matched particle and plotting w/ barcode "<<barcode);
                    //here we plot point of angle vs countsize!
                    float delx = hit.localEndPosition().x() - hit.localStartPosition().x();
                    float dely = hit.localEndPosition().y() - hit.localStartPosition().y();
                    float delz = hit.localEndPosition().z() - hit.localStartPosition().z();
                    float norm = sqrt(delx*delx + dely*dely + delz*delz);
                    ATH_MSG_INFO("acos val and group val of "<<acos(abs(delx)/norm)<<" "<<rawdata->getGroupSize());
                    float ang = acos(delx/norm);
                    if(ang > 1.5)
                    {
                        ang = ang - 3.1415;
                    }
                    m_hprof->Fill(ang, rawdata->getGroupSize(),1);
                    m_incAnglHist->Fill(ang);
                    break;
                }
            }
        }
    }

    return StatusCode::SUCCESS;
}

StatusCode RDOReadAlg::finalize()
{
    return StatusCode::SUCCESS;
}